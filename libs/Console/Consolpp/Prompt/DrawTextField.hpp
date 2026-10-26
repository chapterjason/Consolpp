#pragma once

#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <cstddef>

namespace Consolpp::Prompt
{
    // Replaces the lines drawn before, with the cursor rowsAboveCursor rows below the first; returns the rows above the
    // cursor now.
    std::size_t drawTextField(const TextField& field, Stream::OutputStream& output, std::size_t rowsAboveCursor);
} // namespace Consolpp::Prompt
