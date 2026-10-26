#pragma once

#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

namespace Consolpp::Stream
{
    // Buffers everything written to the stream while it lives, so what is flushed then reaches the descriptor in one
    // piece; at its end the stream gets its flush mode back. Must end before the stream.
    class BufferingScope
    {
        private:
            OutputStream* output;
            FlushMode mode;

        public:
            explicit BufferingScope(OutputStream& output)
                : output{&output},
                  mode{output.getFlushMode()}
            {
                output.setFlushMode(FlushMode::Buffered);
            }

            BufferingScope(const BufferingScope&) = delete;
            BufferingScope(BufferingScope&&) = delete;
            BufferingScope& operator=(const BufferingScope&) = delete;
            BufferingScope& operator=(BufferingScope&&) = delete;

            ~BufferingScope()
            {
                this->output->setFlushMode(this->mode);
            }
    };
} // namespace Consolpp::Stream
