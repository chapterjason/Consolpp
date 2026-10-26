#pragma once

#include <cstddef>

namespace Consolpp::Text
{
    struct WrappedLine
    {
        public:
            std::size_t start;
            std::size_t end;
            // The display width of the line.
            std::size_t width;

            constexpr bool operator==(const WrappedLine& other) const = default;
    };
} // namespace Consolpp::Text
