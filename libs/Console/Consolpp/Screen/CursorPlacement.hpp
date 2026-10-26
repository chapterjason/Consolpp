#pragma once

#include <cstdint>

namespace Consolpp::Screen
{
    // Where the cursor rests between the draws of a section stack.
    enum class CursorPlacement : std::uint8_t
    {
        // At the start of the first row. Terminals carry the cursor along with its text when they reflow on a resize,
        // so a draw clears from the cursor down without counting the old rows; anything else written meanwhile lands
        // at the top of the stack.
        Top,
        // Below the last row. A draw moves up by the rows the old lines take at the current width, which goes wrong
        // when the terminal reflowed them differently; anything else written meanwhile lands below the stack.
        Below,
    };
} // namespace Consolpp::Screen
