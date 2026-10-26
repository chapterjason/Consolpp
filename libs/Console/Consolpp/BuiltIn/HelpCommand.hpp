#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/BuiltIn/Format.hpp"
#include "Consolpp/BuiltIn/FormatNames.hpp"
#include "Consolpp/BuiltIn/ParseFormat.hpp"
#include "Consolpp/CommandError.hpp"
#include "Consolpp/CommandErrorKind.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/FormatDefinitionAsJson.hpp"
#include "Consolpp/Formatting/FormatDefinitionAsMarkdown.hpp"
#include "Consolpp/Formatting/FormatDefinitionAsText.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::BuiltIn
{
    class HelpCommand
    {
        public:
            static constexpr Definition definition{
                .name = "help",
                .description = "Describes a command",
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
                                .name = "command",
                                .required = false,
                                .description = "The name or alias of the command",
                                .defaultValue = "help",
                                .completion = ValueCompletion::Computed,
                            },
                        },
                    },
            };

            // Throws CommandError for an unknown command and std::invalid_argument for an unknown format.
            static int run(const Parsing::Parameters<definition>& parameters,
                           Stream::OutputStream& output,
                           const CommandList& commands)
            {
                const Format format = BuiltIn::parseFormat(parameters.getOption<"format">());
                const std::string_view name = parameters.getArgument<"command">();
                const auto found = std::ranges::find_if(commands.definitions, [name](const DefinitionView& candidate) {
                    return candidate.name == name
                           || std::ranges::find(candidate.aliases, name) != candidate.aliases.end();
                });

                if (found == commands.definitions.end()) {
                    throw CommandError{CommandErrorKind::UnknownCommand, name, commands.names, commands.isSuggestion};
                }

                if (format == Format::Text) {
                    std::string markup;

                    Formatting::formatDefinitionAsText(*found, std::back_inserter(markup));
                    output << markup;
                } else if (format == Format::Json) {
                    std::string json;

                    Formatting::formatDefinitionAsJson(*found, std::back_inserter(json));
                    json += '\n';
                    output.writeRaw(json);
                } else {
                    std::string markdown;

                    Formatting::formatDefinitionAsMarkdown(*found, std::back_inserter(markdown));
                    output.writeRaw(markdown);
                }

                return 0;
            }

            // The visible names and aliases, for the command argument.
            static std::vector<std::string> complete(const Completion::Request& /*request*/,
                                                     const CommandList& commands)
            {
                return std::vector<std::string>{commands.names.begin(), commands.names.end()};
            }
    };
} // namespace Consolpp::BuiltIn
