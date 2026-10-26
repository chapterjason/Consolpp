#pragma once

#include "Consolpp/Prompt/FormField.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <cstddef>
#include <span>

namespace Consolpp::Prompt
{
    // Replaces the lines drawn before, with the cursor rowsAboveCursor rows below the first; returns the rows above the
    // cursor now. The fields before the active one show their answer, the ones after it their question.
    std::size_t drawForm(std::span<const FormField> fields,
                         std::size_t active,
                         Stream::OutputStream& output,
                         std::size_t rowsAboveCursor);
} // namespace Consolpp::Prompt
