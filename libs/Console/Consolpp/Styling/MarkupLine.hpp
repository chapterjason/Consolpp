#pragma once

#include <cstddef>

namespace Consolpp::Styling
{
    // A line of markup in a text that holds several of them.
    struct MarkupLine
    {
        public:
            std::size_t start;
            std::size_t length;
            // The display width of the visible text of the line.
            std::size_t width;

            constexpr bool operator==(const MarkupLine& other) const = default;
    };
} // namespace Consolpp::Styling
