#pragma once

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/Field.hpp"
#include "Consolpp/Prompt/TextField.hpp"

#include <concepts>
#include <cstddef>
#include <string>
#include <type_traits>
#include <vector>

namespace Consolpp::Prompt
{
    // What asking the field returns.
    template <Field Type>
    using Answer = std::conditional_t<
        std::same_as<Type, TextField>,
        std::string,
        std::conditional_t<std::same_as<Type, ChoiceField>, std::vector<std::size_t>, bool>>;
} // namespace Consolpp::Prompt
