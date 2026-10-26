#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Screen/Record.hpp"

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Tools::Screen
{
    struct RecordCommand
    {
        public:
            static constexpr Definition definition{
                .name = "record",
                .description = "Runs a command and records its output and the window sizes into a file.",
                .usages = std::array<std::string_view, 1>{"record table.log -- ./build/debug/bin/consolpp_demo table"},
                .signature =
                    Signature{
                        std::array<Option, 0>{},
                        std::array{
                            Argument{.name = "file", .description = "The file the recording is written to"},
                            Argument{.name = "command", .variadic = true, .description = "The command to record"},
                        },
                    },
            };

            static int run(const Parsing::Parameters<definition>& parameters, Event::Loop& loop)
            {
                std::vector<std::string> command;

                for (const std::string_view argument : parameters.getArgument<"command">()) {
                    command.emplace_back(argument);
                }

                return Screen::record(command, std::string{parameters.getArgument<"file">()}, loop);
            }
    };
} // namespace Consolpp::Tools::Screen
