#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <string_view>

namespace Demo::Single
{
    // The only command of its program: all of argv belongs to it, so it is run without a command name.
    class GreetCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "greet",
                .description = "Greets someone",
                .usages =
                    std::array<std::string_view, 3>{
                        "Alice",
                        "Alice --shout",
                        "-- completion",
                    },
                .signature =
                    Consolpp::Signature{
                        std::array{Consolpp::Option{.name = "shout", .shortcut = 's', .description = "Greets loudly"}},
                        std::array{Consolpp::Argument{.name = "name", .description = "Who to greet"}},
                    },
            };

            static int run(const Consolpp::Parsing::Parameters<definition>& parameters,
                           Consolpp::Stream::OutputStream& output)
            {
                const std::string_view name = parameters.getArgument<"name">();

                if (parameters.getOption<"shout">().value_or(false)) {
                    output.write("<info>HELLO, {}!</info>\n", name);

                    return 0;
                }

                output.write("Hello, <info>{}</info>.\n", name);

                return 0;
            }
    };
} // namespace Demo::Single
