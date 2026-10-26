#pragma once

#include "Consolpp/Stream/OutputStream.hpp"

namespace Consolpp::Terminal
{
    // Hides the cursor of the terminal for its lifetime and shows it again at the end, also after an exception.
    class HiddenCursor
    {
        private:
            Stream::OutputStream* output;

        public:
            explicit HiddenCursor(Stream::OutputStream& output);

            HiddenCursor(const HiddenCursor&) = delete;
            HiddenCursor(HiddenCursor&&) = delete;
            HiddenCursor& operator=(const HiddenCursor&) = delete;
            HiddenCursor& operator=(HiddenCursor&&) = delete;

            ~HiddenCursor();
    };
} // namespace Consolpp::Terminal
