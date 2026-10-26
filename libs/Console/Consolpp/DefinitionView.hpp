#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"

#include <cstddef>
#include <span>
#include <string_view>

namespace Consolpp
{
    struct DefinitionView
    {
        public:
            std::string_view name;
            std::string_view description{};
            std::string_view help{};
            std::span<const std::string_view> aliases{};
            std::span<const std::string_view> usages{};
            std::span<const Option> options{};
            std::span<const Argument> arguments{};
            bool hidden{false};

            // The definition must outlive the view, as the definitions of commands do.
            template <std::size_t OptionCount,
                      std::size_t ArgumentCount,
                      std::size_t AliasCount,
                      std::size_t UsageCount>
            static constexpr DefinitionView fromDefinition(
                const Definition<OptionCount, ArgumentCount, AliasCount, UsageCount>& definition)
            {
                return DefinitionView{
                    .name = definition.name,
                    .description = definition.description,
                    .help = definition.help,
                    .aliases = definition.aliases,
                    .usages = definition.usages,
                    .options = definition.signature.getOptions(),
                    .arguments = definition.signature.getArguments(),
                    .hidden = definition.hidden,
                };
            }
    };
} // namespace Consolpp
