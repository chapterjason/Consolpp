#pragma once

#include "Consolpp/Stream/OutputStream.hpp"

namespace Consolpp::Terminal
{
    // Turns bracketed paste on for its lifetime, also ending after an exception.
    class BracketedPaste
    {
        private:
            Stream::OutputStream* output;

        public:
            explicit BracketedPaste(Stream::OutputStream& output);

            BracketedPaste(const BracketedPaste&) = delete;
            BracketedPaste(BracketedPaste&&) = delete;
            BracketedPaste& operator=(const BracketedPaste&) = delete;
            BracketedPaste& operator=(BracketedPaste&&) = delete;

            ~BracketedPaste();
    };
} // namespace Consolpp::Terminal
