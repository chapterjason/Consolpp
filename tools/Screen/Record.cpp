#include "Screen/Record.hpp"

#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Event/Registration.hpp"
#include "Consolpp/Event/SourceKind.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Stream/ReadDescriptor.hpp"
#include "Consolpp/Terminal/GetColumns.hpp"
#include "Consolpp/Terminal/GetRows.hpp"
#include "Consolpp/Terminal/RawMode.hpp"
#include "Screen/Entry.hpp"
#include "Screen/EntryKind.hpp"
#include "Screen/FormatEntry.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <ios>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#ifdef __APPLE__
#    include <util.h>
#else
#    include <pty.h>
#endif

namespace Consolpp::Tools::Screen
{
    namespace
    {
        constexpr std::size_t bufferSize = 65536;
        constexpr int execFailed = 127;

        winsize getSize()
        {
            constexpr std::size_t limit = std::numeric_limits<unsigned short>::max();

            return winsize{
                .ws_row = static_cast<unsigned short>(std::min(Terminal::getRows(STDOUT_FILENO), limit)),
                .ws_col = static_cast<unsigned short>(std::min(Terminal::getColumns(STDOUT_FILENO), limit)),
                .ws_xpixel = 0,
                .ws_ypixel = 0,
            };
        }

        [[noreturn]]
        void runCommand(const std::span<const std::string> command)
        {
            std::vector<std::string> copies{command.begin(), command.end()};
            std::vector<char*> arguments;

            arguments.reserve(copies.size() + 1);

            for (std::string& argument : copies) {
                arguments.push_back(argument.data());
            }

            arguments.push_back(nullptr);
            execvp(arguments.front(), arguments.data());
            _exit(execFailed);
        }
    } // namespace

    int record(const std::span<const std::string> command, const std::string& path, Event::Loop& loop)
    {
        if (command.empty()) {
            throw std::invalid_argument{"There is no command to record"};
        }

        std::ofstream log{path};

        if (!log) {
            throw std::system_error{errno, std::generic_category(), "The recording file cannot be written"};
        }

        log.exceptions(std::ios_base::failbit | std::ios_base::badbit);

        winsize size = getSize(); // NOLINT(misc-const-correctness): macOS forkpty() takes a non-const winsize
        int master = -1;
        const auto child = forkpty(&master, nullptr, nullptr, &size);

        if (child < 0) {
            throw std::system_error{errno, std::generic_category()};
        }

        if (child == 0) {
            runCommand(command);
        }

        const auto start = std::chrono::steady_clock::now();
        const auto writeEntry = [&log, &start](Entry entry) {
            entry.time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()
                                                                               - start);
            log << Screen::formatEntry(entry) << '\n';
        };
        std::array<char, bufferSize> buffer{};
        std::optional<Event::Registration> input;

        writeEntry(Entry{.kind = EntryKind::Resize, .columns = size.ws_col, .rows = size.ws_row});

        {
            const Terminal::RawMode raw{STDIN_FILENO};
            Stream::OutputStream terminal{STDOUT_FILENO, Stream::FlushMode::Immediate};
            Stream::OutputStream commandInput{master, Stream::FlushMode::Immediate};
            const Event::Registration output = loop.registerReadable(
                master,
                [&loop, &buffer, &master, &terminal, &writeEntry] {
                    std::size_t got = 0;

                    try {
                        got = Stream::readDescriptor(master, buffer);
                    } catch (const std::system_error& error) {
                        // The pseudo terminal reports EIO once the command has ended.
                        if (error.code() != std::errc::io_error) {
                            throw;
                        }
                    }

                    if (got == 0) {
                        loop.stop();

                        return;
                    }

                    const std::string_view bytes{buffer.data(), got};

                    terminal.writeRaw(bytes);
                    writeEntry(Entry{.kind = EntryKind::Output, .bytes = std::string{bytes}});
                });
            const Event::Registration resize = loop.registerSignal(SIGWINCH, [&master, &writeEntry] {
                const winsize current = getSize();

                if (ioctl(master, TIOCSWINSZ, &current) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                writeEntry(Entry{.kind = EntryKind::Resize, .columns = current.ws_col, .rows = current.ws_row});
            });

            // Only a terminal can be watched for keys; without one there are none to pass on.
            if (isatty(STDIN_FILENO) == 1) {
                input.emplace(loop,
                              Event::SourceKind::Readable,
                              STDIN_FILENO,
                              std::chrono::milliseconds{0},
                              [&input, &buffer, &commandInput] {
                                  const std::size_t got = Stream::readDescriptor(STDIN_FILENO, buffer);

                                  if (got == 0) {
                                      input.reset();

                                      return;
                                  }

                                  commandInput.writeRaw(std::string_view{buffer.data(), got});
                              });
            }

            loop.run();
            input.reset();
        }

        int status = 0;

        if (close(master) != 0) {
            throw std::system_error{errno, std::generic_category()};
        }

        if (waitpid(child, &status, 0) < 0) {
            throw std::system_error{errno, std::generic_category()};
        }

        log.close();

        // <sys/wait.h> declares the macros, but <stdlib.h> defined them first through the standard headers.
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1; // NOLINT(misc-include-cleaner)
    }
} // namespace Consolpp::Tools::Screen
