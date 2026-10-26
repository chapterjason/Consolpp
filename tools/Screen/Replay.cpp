#include "Screen/Replay.hpp"

#include "Consolpp/Stream/OutputStream.hpp"
#include "Screen/Emulator.hpp"
#include "Screen/Entry.hpp"
#include "Screen/EntryKind.hpp"
#include "Screen/ParseEntry.hpp"

#include <cerrno>
#include <cstddef>
#include <format>
#include <fstream>
#include <ios>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace Consolpp::Tools::Screen
{
    void replay(const std::string& path, const bool scrollback, Stream::OutputStream& output)
    {
        constexpr long long microsecondsPerSecond = 1'000'000;
        std::ifstream file{path};
        std::string line;
        std::optional<Emulator> emulator;
        std::string previous;
        std::size_t frame = 0;

        if (!file) {
            throw std::system_error{errno, std::generic_category(), "The recording cannot be read"};
        }

        file.exceptions(std::ios_base::badbit);

        while (std::getline(file, line)) {
            const Entry entry = Screen::parseEntry(line);
            const bool resize = entry.kind == EntryKind::Resize;

            if (!emulator.has_value() && !resize) {
                throw std::invalid_argument{"A recording must start with the size of the terminal"};
            }

            if (!emulator.has_value()) {
                emulator.emplace(entry.columns, entry.rows);
            } else if (resize) {
                emulator->resize(entry.columns, entry.rows);
            } else {
                emulator->write(entry.bytes);
            }

            std::string screen = emulator->render(scrollback);

            if (screen == previous) {
                continue;
            }

            ++frame;
            output.writeRaw(std::format("--- frame {} at {}.{:06} s after {} ---\n",
                                        frame,
                                        entry.time.count() / microsecondsPerSecond,
                                        entry.time.count() % microsecondsPerSecond,
                                        resize ? "a resize" : "output"));
            output.writeRaw(screen);
            previous = std::move(screen);
        }

        output.flush();
    }
} // namespace Consolpp::Tools::Screen
