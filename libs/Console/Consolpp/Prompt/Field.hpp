#pragma once

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/TextField.hpp"

#include <concepts>

namespace Consolpp::Prompt
{
    template <typename Type>
    concept Field = std::same_as<Type, TextField>
                    || std::same_as<Type, ChoiceField>
                    || std::same_as<Type, ConfirmationField>;
} // namespace Consolpp::Prompt
