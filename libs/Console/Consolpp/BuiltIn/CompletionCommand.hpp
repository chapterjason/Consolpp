#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Completion/FormatBashScript.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Consolpp::BuiltIn
{
    inline constexpr std::array<std::string_view, 1> shellNames{"bash"};

    class CompletionCommand
    {
        public:
            static constexpr Definition definition{
                .name = "completion",
                .description = "Prints the completion script of the program for the shell",
                .help = "Load the script in the .bashrc with: eval \"$(program completion bash)\", where program is "
                        "the name of the program.",
                .hidden = true,
                .signature =
                    Signature{
                        std::array<Option, 0>{},
                        std::array{
                            Argument{.name = "shell", .description = "The shell: bash", .suggestedValues = shellNames},
                        },
                    },
            };

            // Throws std::invalid_argument for a shell other than bash.
            static int run(const Parsing::Parameters<definition>& parameters,
                           Stream::OutputStream& output,
                           const Program& program)
            {
                std::string script;

                if (parameters.getArgument<"shell">() != "bash") {
                    throw std::invalid_argument{"The shell has to be bash"};
                }

                Completion::formatBashScript(program.name, std::back_inserter(script));
                output.writeRaw(script);

                return 0;
            }
    };
} // namespace Consolpp::BuiltIn
