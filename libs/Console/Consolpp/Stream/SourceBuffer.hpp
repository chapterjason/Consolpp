#pragma once

#include <array>
#include <cstddef>
#include <streambuf>

namespace Consolpp::Stream
{
    class SourceBuffer : public std::streambuf
    {
        private:
            static constexpr std::size_t capacity = 65536;

            std::array<char, capacity> buffer{};
            int descriptor;

        protected:
            int_type underflow() override;

        public:
            explicit SourceBuffer(const int descriptor);

            SourceBuffer(const SourceBuffer&) = delete;
            SourceBuffer(SourceBuffer&&) = delete;
            SourceBuffer& operator=(const SourceBuffer&) = delete;
            SourceBuffer& operator=(SourceBuffer&&) = delete;

            ~SourceBuffer() override = default;
    };
} // namespace Consolpp::Stream
