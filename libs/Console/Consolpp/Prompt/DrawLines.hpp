#pragma once

#include "Consolpp/Stream/OutputStream.hpp"

#include <cstddef>
#include <span>
#include <string>

namespace Consolpp::Prompt
{
    // Replaces the lines drawn before, with the cursor rowsAboveCursor rows below the first, by the markup lines and
    // puts the cursor at the column of the cursor line; returns the rows above the cursor now.
    std::size_t drawLines(std::span<const std::string> lines,
                          std::size_t cursorLine,
                          std::size_t cursorColumn,
                          Stream::OutputStream& output,
                          std::size_t rowsAboveCursor);
} // namespace Consolpp::Prompt
