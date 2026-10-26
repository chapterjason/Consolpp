#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <array>

namespace Consolpp::BuiltIn
{
    class ManCommand
    {
        public:
            static constexpr Definition definition{
                .name = "man",
                .description = "Generates the man page of the program",
                .hidden = true,
                .signature =
                    Signature{
                        std::array<Option, 0>{},
                        std::array{
                            Argument{
                                .name = "file",
                                .required = false,
                                .description = "Writes the page to this file instead of the output",
                                .completion = ValueCompletion::File,
                            },
                        },
                    },
            };

            // Throws std::system_error when the file cannot be written.
            static int run(const Parsing::Parameters<definition>& parameters,
                           Stream::OutputStream& output,
                           const CommandList& commands,
                           const Program& program);
    };
} // namespace Consolpp::BuiltIn
