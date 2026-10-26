#pragma once

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/TextField.hpp"

#include <variant>

namespace Consolpp::Prompt
{
    using FormField = std::variant<TextField, ChoiceField, ConfirmationField>;
} // namespace Consolpp::Prompt
