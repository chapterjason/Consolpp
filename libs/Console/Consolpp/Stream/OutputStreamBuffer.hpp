#pragma once

#include "Consolpp/Stream/OutputStream.hpp"

#include <ios>
#include <streambuf>

namespace Consolpp::Stream
{
    class OutputStreamBuffer : public std::streambuf
    {
        private:
            OutputStream* writer;

        protected:
            int_type overflow(int_type character) override;

            std::streamsize xsputn(const char_type* text, std::streamsize count) override;

            int sync() override;

        public:
            explicit OutputStreamBuffer(OutputStream& writer);

            OutputStreamBuffer(const OutputStreamBuffer&) = delete;
            OutputStreamBuffer(OutputStreamBuffer&&) = delete;
            OutputStreamBuffer& operator=(const OutputStreamBuffer&) = delete;
            OutputStreamBuffer& operator=(OutputStreamBuffer&&) = delete;

            ~OutputStreamBuffer() override = default;
    };
} // namespace Consolpp::Stream
