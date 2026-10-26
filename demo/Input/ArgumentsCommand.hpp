#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Log.hpp"

#include <array>
#include <sstream>
#include <string_view>

namespace Demo::Input
{
    class ArgumentsCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "input:arguments",
                .description = "Prints the options and arguments as they were parsed",
                .help = "Leave out the source to be asked for it, as a missing required argument is asked for.",
                .usages =
                    std::array<std::string_view, 3>{
                        "input:arguments app.tar",
                        "input:arguments -f --no-color -o build -i src -i lib app.tar /srv one two",
                        "input:arguments --tag -d DEBUG -dLEVEL=2 app.tar",
                    },
                .signature =
                    Consolpp::Signature{
                        std::array{
                            Consolpp::Option{.name = "force", .shortcut = 'f', .description = "A flag"},
                            Consolpp::Option{
                                .name = "color",
                                .description = "A flag that is on unless negated",
                                .defaultEnabled = true,
                            },
                            Consolpp::Option{
                                .name = "output",
                                .shortcut = 'o',
                                .value = Consolpp::OptionValue::Required,
                                .description = "A required value with a default",
                                .defaultValue = "dist",
                            },
                            Consolpp::Option{
                                .name = "tag",
                                .shortcut = 't',
                                .value = Consolpp::OptionValue::Optional,
                                .description = "A value that may be left out",
                            },
                            Consolpp::Option{
                                .name = "include",
                                .shortcut = 'i',
                                .value = Consolpp::OptionValue::Required,
                                .repeatable = true,
                                .description = "A repeatable value",
                            },
                            Consolpp::Option{
                                .name = "define",
                                .shortcut = 'd',
                                .value = Consolpp::OptionValue::Optional,
                                .repeatable = true,
                                .description = "A repeatable value that may be left out",
                            },
                        },
                        std::array{
                            Consolpp::Argument{.name = "source", .description = "A required argument"},
                            Consolpp::Argument{
                                .name = "target",
                                .required = false,
                                .description = "An optional argument with a default",
                                .defaultValue = "/var/www",
                            },
                            Consolpp::Argument{
                                .name = "extras",
                                .required = false,
                                .variadic = true,
                                .description = "Any number of further values",
                            },
                        },
                    },
            };

            static int run(const Consolpp::Parsing::Parameters<definition>& parameters,
                           Consolpp::Stream::OutputStream& output)
            {
                const auto show = [&output](const std::string_view name, const auto& value) {
                    std::ostringstream text;

                    Demo::write(text, value);
                    output.write("<comment>{:<10}</comment> {}\n", name, text.str());
                };

                show("force", parameters.getOption<"force">());
                show("color", parameters.getOption<"color">());
                show("output", parameters.getOption<"output">());
                show("tag", parameters.getOption<"tag">());
                show("include", parameters.getOption<"include">());
                show("define", parameters.getOption<"define">());
                show("source", parameters.getArgument<"source">());
                show("target", parameters.getArgument<"target">());
                show("extras", parameters.getArgument<"extras">());

                return 0;
            }
    };
} // namespace Demo::Input
