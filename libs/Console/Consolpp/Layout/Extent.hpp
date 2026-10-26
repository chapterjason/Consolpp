#pragma once

#include <cstddef>

namespace Consolpp::Layout
{
    // The widths an element can be drawn at, in display columns.
    struct Extent
    {
        public:
            // Narrower, the element breaks words or gets wider than it was given.
            std::size_t minimum;
            // The width the element takes when it has the room.
            std::size_t natural;

            constexpr bool operator==(const Extent& other) const = default;
    };
} // namespace Consolpp::Layout
