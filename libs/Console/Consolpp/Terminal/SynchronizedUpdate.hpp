#pragma once

#include "Consolpp/Stream/OutputStream.hpp"

namespace Consolpp::Terminal
{
    // Asks the terminal to show what is written during its lifetime at once at the end, so a redraw does not flicker
    // (synchronized output, mode 2026); terminals without it ignore the request. Flushes nothing itself.
    class SynchronizedUpdate
    {
        private:
            Stream::OutputStream* output;

        public:
            explicit SynchronizedUpdate(Stream::OutputStream& output);

            SynchronizedUpdate(const SynchronizedUpdate&) = delete;
            SynchronizedUpdate(SynchronizedUpdate&&) = delete;
            SynchronizedUpdate& operator=(const SynchronizedUpdate&) = delete;
            SynchronizedUpdate& operator=(SynchronizedUpdate&&) = delete;

            ~SynchronizedUpdate();
    };
} // namespace Consolpp::Terminal
