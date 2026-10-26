#pragma once

#include "Consolpp/Prompt/InputLineText.hpp"
#include "Consolpp/Prompt/TextField.hpp"

namespace Consolpp::Prompt
{
    // Masked text as one "*" per grapheme, hidden text as nothing, and control characters as U+FFFD.
    InputLineText createInputLineText(const TextField& field);
} // namespace Consolpp::Prompt
