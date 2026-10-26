#pragma once

#include <optional>

#include <termios.h>

namespace Consolpp::Terminal
{
    // Switches the terminal behind the descriptor to raw input for its lifetime: no line buffering, no echo and no
    // signals from keys, so every key arrives as it is typed. Does nothing for a descriptor that is no terminal.
    class RawMode
    {
        private:
            int descriptor;
            std::optional<termios> previous;

        public:
            // Throws std::system_error when the terminal cannot be switched.
            explicit RawMode(int descriptor);

            RawMode(const RawMode&) = delete;
            RawMode(RawMode&&) = delete;
            RawMode& operator=(const RawMode&) = delete;
            RawMode& operator=(RawMode&&) = delete;

            ~RawMode();
    };
} // namespace Consolpp::Terminal
