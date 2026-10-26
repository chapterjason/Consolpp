#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Screen/Replay.hpp"

#include <array>
#include <string>
#include <string_view>

namespace Consolpp::Tools::Screen
{
    struct ReplayCommand
    {
        public:
            static constexpr Definition definition{
                .name = "replay",
                .description = "Plays a recording back and shows the screen after every change.",
                .usages = std::array<std::string_view, 1>{"replay table.log --scrollback"},
                .signature =
                    Signature{
                        std::array{
                            Option{.name = "scrollback", .shortcut = 's', .description = "Shows the scrollback too"},
                        },
                        std::array{
                            Argument{.name = "file", .description = "The recording"},
                        },
                    },
            };

            static int run(const Parsing::Parameters<definition>& parameters, Stream::OutputStream& output)
            {
                Screen::replay(std::string{parameters.getArgument<"file">()},
                               parameters.getOption<"scrollback">().value_or(false),
                               output);

                return 0;
            }
    };
} // namespace Consolpp::Tools::Screen
