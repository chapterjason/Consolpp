#pragma once

#include <cstddef>
#include <cstdint>

namespace Consolpp::Text::Utf8
{
    struct CodePoint
    {
        public:
            std::uint32_t value;
            std::size_t length;

            constexpr bool operator==(const CodePoint& other) const = default;
    };
} // namespace Consolpp::Text::Utf8
