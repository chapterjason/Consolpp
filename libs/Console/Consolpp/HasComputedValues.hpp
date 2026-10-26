#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <algorithm>
#include <cstddef>

namespace Consolpp
{
    // Whether an option or argument completes its values through the complete member of the command.
    template <std::size_t OptionCount, std::size_t ArgumentCount, std::size_t AliasCount, std::size_t UsageCount>
    consteval bool hasComputedValues(const Definition<OptionCount, ArgumentCount, AliasCount, UsageCount>& definition)
    {
        return std::ranges::any_of(definition.signature.getOptions(),
                                   [](const Option& option) {
                                       return option.completion == ValueCompletion::Computed;
                                   })
               || std::ranges::any_of(definition.signature.getArguments(), [](const Argument& argument) {
                      return argument.completion == ValueCompletion::Computed;
                  });
    }
} // namespace Consolpp
