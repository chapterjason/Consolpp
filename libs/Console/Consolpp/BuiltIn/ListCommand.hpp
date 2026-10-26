#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/BuiltIn/Format.hpp"
#include "Consolpp/BuiltIn/FormatNames.hpp"
#include "Consolpp/BuiltIn/ParseFormat.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Formatting/FormatDefinitionsAsJson.hpp"
#include "Consolpp/Formatting/FormatDefinitionsAsMarkdown.hpp"
#include "Consolpp/Formatting/FormatDefinitionsAsText.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::BuiltIn
{
    class ListCommand
    {
        public:
            static constexpr Definition definition{
                .name = "list",
                .description = "Lists the commands",
                .signature =
                    Signature{
                        std::array{
                            Option{
                                .name = "format",
                                .value = OptionValue::Required,
                                .description = "The format: txt, json or md",
                                .defaultValue = "txt",
                                .suggestedValues = formatNames,
                            },
                        },
                        std::array{
                            Argument{
                                .name = "namespace",
                                .required = false,
                                .description = "Lists only the commands of this namespace",
                                .completion = ValueCompletion::Computed,
                            },
                        },
                    },
            };

            // Throws CommandError for a namespace without commands and std::invalid_argument for an unknown format.
            static int run(const Parsing::Parameters<definition>& parameters,
                           Stream::OutputStream& output,
                           const CommandList& commands)
            {
                const Format format = BuiltIn::parseFormat(parameters.getOption<"format">());
                const std::string_view namespaceName = parameters.getArgument<"namespace">().value_or(
                    std::string_view{});

                if (format == Format::Text) {
                    std::string markup;

                    Formatting::formatDefinitionsAsText(commands.definitions,
                                                        namespaceName,
                                                        std::back_inserter(markup));
                    output << markup;
                } else if (format == Format::Json) {
                    std::string json;

                    Formatting::formatDefinitionsAsJson(commands.definitions, namespaceName, std::back_inserter(json));
                    json += '\n';
                    output.writeRaw(json);
                } else {
                    std::string markdown;

                    Formatting::formatDefinitionsAsMarkdown(commands.definitions,
                                                            namespaceName,
                                                            std::back_inserter(markdown));
                    output.writeRaw(markdown);
                }

                return 0;
            }

            // Every part before a colon of the visible names and aliases, for the namespace argument.
            static std::vector<std::string> complete(const Completion::Request& /*request*/,
                                                     const CommandList& commands)
            {
                std::vector<std::string> namespaces;

                for (const std::string_view name : commands.names) {
                    for (std::size_t colon = name.find(':'); colon != std::string_view::npos;
                         colon = name.find(':', colon + 1)) {
                        const std::string_view namespaceName = name.substr(0, colon);

                        if (std::ranges::find(namespaces, namespaceName) == namespaces.end()) {
                            namespaces.emplace_back(namespaceName);
                        }
                    }
                }

                return namespaces;
            }
    };
} // namespace Consolpp::BuiltIn
