#pragma once

#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <cstddef>

namespace Consolpp::Prompt
{
    // Replaces the line drawn before, with the cursor rowsAboveCursor rows below its start; returns the rows above the
    // cursor now.
    std::size_t drawConfirmationField(const ConfirmationField& field,
                                      Stream::OutputStream& output,
                                      std::size_t rowsAboveCursor);
} // namespace Consolpp::Prompt
