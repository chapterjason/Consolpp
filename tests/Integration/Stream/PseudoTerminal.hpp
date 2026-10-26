#pragma once

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <string_view>
#include <system_error>

#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

namespace Consolpp::Tests::Integration::Stream
{
    class PseudoTerminal
    {
        private:
            static constexpr std::size_t nameCapacity = 128;
            static constexpr std::size_t chunkSize = 4096;
            // Linux hands pty output to the master side asynchronously.
            static constexpr int waitMilliseconds = 1000;
            static constexpr int quietMilliseconds = 50;

            int master;
            int terminal{-1};

        public:
            PseudoTerminal()
                : master{posix_openpt(O_RDWR | O_NOCTTY)}
            {
                if (this->master < 0 || grantpt(this->master) != 0 || unlockpt(this->master) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                std::array<char, PseudoTerminal::nameCapacity> name{};

                if (ptsname_r(this->master, name.data(), name.size()) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                this->terminal = open(name.data(), O_RDWR | O_NOCTTY);

                if (this->terminal < 0) {
                    throw std::system_error{errno, std::generic_category()};
                }
            }

            PseudoTerminal(const PseudoTerminal&) = delete;
            PseudoTerminal(PseudoTerminal&&) = delete;
            PseudoTerminal& operator=(const PseudoTerminal&) = delete;
            PseudoTerminal& operator=(PseudoTerminal&&) = delete;

            ~PseudoTerminal()
            {
                close(this->terminal);
                close(this->master);
            }

            [[nodiscard]]
            int getTerminal() const
            {
                return this->terminal;
            }

            // Bytes typed before a prompt switches to raw mode would meet the line discipline otherwise.
            void switchToRaw() const
            {
                termios settings{};

                if (tcgetattr(this->terminal, &settings) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                cfmakeraw(&settings);

                if (tcsetattr(this->terminal, TCSANOW, &settings) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }
            }

            // Writes the bytes as if they were typed into the terminal.
            void type(const std::string_view bytes) const
            {
                if (write(this->master, bytes.data(), bytes.size()) != static_cast<ssize_t>(bytes.size())) {
                    throw std::system_error{errno, std::generic_category()};
                }
            }

            void setColumns(const unsigned short columns) const
            {
                const winsize size{.ws_row = 24, .ws_col = columns, .ws_xpixel = 0, .ws_ypixel = 0};

                if (ioctl(this->master, TIOCSWINSZ, &size) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }
            }

            void setRows(const unsigned short rows) const
            {
                winsize size{};

                if (ioctl(this->master, TIOCGWINSZ, &size) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                size.ws_row = rows;

                if (ioctl(this->master, TIOCSWINSZ, &size) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }
            }

            // Reads until nothing more arrives for a moment, as the output can come in several pieces.
            [[nodiscard]]
            std::string drain() const
            {
                std::string received;
                pollfd ready{.fd = this->master, .events = POLLIN, .revents = 0};
                int timeout = PseudoTerminal::waitMilliseconds;

                while (poll(&ready, 1, timeout) > 0) {
                    std::array<char, PseudoTerminal::chunkSize> chunk{};
                    const ssize_t got = read(this->master, chunk.data(), chunk.size());

                    if (got <= 0) {
                        break;
                    }

                    received.append(chunk.data(), static_cast<std::size_t>(got));
                    timeout = PseudoTerminal::quietMilliseconds;
                }

                return received;
            }
    };
} // namespace Consolpp::Tests::Integration::Stream
