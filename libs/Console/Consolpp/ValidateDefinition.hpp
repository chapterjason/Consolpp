#pragma once

#include "Consolpp/Definition.hpp"
#include "Consolpp/Markup/ValidateSyntax.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace Consolpp
{
    template <std::size_t OptionCount, std::size_t ArgumentCount, std::size_t AliasCount, std::size_t UsageCount>
    consteval bool validateDefinition(const Definition<OptionCount, ArgumentCount, AliasCount, UsageCount>& definition)
    {
        const auto validateName = [](const std::string_view name) {
            if (name.empty()) {
                throw std::invalid_argument{"A command name or alias must not be empty"};
            }

            if (name.starts_with(':') || name.ends_with(':') || name.find("::") != std::string_view::npos) {
                throw std::invalid_argument{"A command name or alias must not have an empty part around a colon"};
            }

            if (name == "_complete") {
                throw std::invalid_argument{"The command name or alias _complete is taken by shell completion"};
            }
        };

        validateName(definition.name);
        Markup::validateSyntax(definition.description);
        Markup::validateSyntax(definition.help);

        for (const std::string_view alias : definition.aliases) {
            validateName(alias);

            if (alias == definition.name) {
                throw std::invalid_argument{"An alias must not equal the command name"};
            }

            if (std::ranges::count(definition.aliases, alias) > 1) {
                throw std::invalid_argument{"Two aliases must not be equal"};
            }
        }

        return true;
    }
} // namespace Consolpp
