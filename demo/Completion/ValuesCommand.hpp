#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace Demo::Completion
{
    inline constexpr std::array<std::string_view, 3> formats{"txt", "json", "md"};

    class ValuesCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "completion:values",
                .description = "Completes the values of its options and arguments in Bash",
                .help = "Load the completion with: eval \"$(consolpp_demo completion bash)\", then press Tab after "
                        "an option or at an argument.",
                .signature =
                    Consolpp::Signature{
                        std::array{
                            Consolpp::Option{
                                .name = "format",
                                .value = Consolpp::OptionValue::Required,
                                .description = "Suggested values: txt, json or md",
                                .suggestedValues = formats,
                            },
                            Consolpp::Option{
                                .name = "environment",
                                .shortcut = 'e',
                                .value = Consolpp::OptionValue::Required,
                                .description = "Values the command computes",
                                .completion = Consolpp::ValueCompletion::Computed,
                            },
                            Consolpp::Option{
                                .name = "config",
                                .value = Consolpp::OptionValue::Required,
                                .description = "A file",
                                .completion = Consolpp::ValueCompletion::File,
                            },
                            Consolpp::Option{
                                .name = "target",
                                .value = Consolpp::OptionValue::Required,
                                .description = "A directory",
                                .completion = Consolpp::ValueCompletion::Directory,
                            },
                        },
                        std::array{
                            Consolpp::Argument{
                                .name = "branch",
                                .description = "Values the command computes",
                                .completion = Consolpp::ValueCompletion::Computed,
                            },
                            Consolpp::Argument{
                                .name = "paths",
                                .required = false,
                                .variadic = true,
                                .description = "Files and directories",
                                .completion = Consolpp::ValueCompletion::Path,
                            },
                        },
                    },
            };

            static int run(const Consolpp::Parsing::Parameters<definition>& parameters,
                           Consolpp::Stream::OutputStream& output)
            {
                output.write("<comment>format</comment>      {}\n",
                             parameters.getOption<"format">().value_or("(none)"));
                output.write("<comment>environment</comment> {}\n",
                             parameters.getOption<"environment">().value_or("(none)"));
                output.write("<comment>config</comment>      {}\n",
                             parameters.getOption<"config">().value_or("(none)"));
                output.write("<comment>target</comment>      {}\n",
                             parameters.getOption<"target">().value_or("(none)"));
                output.write("<comment>branch</comment>      {}\n", parameters.getArgument<"branch">());

                for (const std::string_view path : parameters.getArgument<"paths">()) {
                    output.write("<comment>path</comment>        {}\n", path);
                }

                return 0;
            }

            static std::vector<std::string> complete(const Consolpp::Completion::Request& request)
            {
                if (request.name == "environment") {
                    return std::vector<std::string>{"development", "staging", "production"};
                }

                return std::vector<std::string>{"main", "develop", "feature/completion", "feature/forms"};
            }
    };
} // namespace Demo::Completion
