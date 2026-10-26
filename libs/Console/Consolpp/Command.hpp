#pragma once

#include "Consolpp/GetCompleteArguments.hpp"
#include "Consolpp/GetRunArguments.hpp"
#include "Consolpp/HasComputedValues.hpp"
#include "Consolpp/ValidateDefinition.hpp"

#include <cstddef>
#include <type_traits>

namespace Consolpp
{
    template <typename Type>
    concept Command = requires { typename std::bool_constant<validateDefinition(Type::definition)>; } && requires {
        typename std::integral_constant<std::size_t, Consolpp::getRunArguments<Type>(&Type::run).size()>;
    } && (!Consolpp::hasComputedValues(Type::definition) || requires {
                          typename std::integral_constant<std::size_t,
                                                          Consolpp::getCompleteArguments(&Type::complete).size()>;
                      });
} // namespace Consolpp
