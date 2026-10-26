#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <cstddef>
#include <string_view>

namespace Demo::Output
{
    class PipeCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "output:pipe",
                .description = "Numbers the lines of the input",
                .help = "Input and output work the same in a terminal, a pipe or a file; in a terminal, Ctrl+D ends "
                        "the input.",
                .usages =
                    std::array<std::string_view, 2>{
                        "output:pipe < file.txt > numbered.txt",
                        "output:pipe < file.txt | cat",
                    },
                .signature = Consolpp::Signature{std::array<Consolpp::Option, 0>{},
                                                 std::array<Consolpp::Argument, 0>{}},
            };

            static int run(Consolpp::Stream::InputStream& input,
                           Consolpp::Stream::OutputStream& output,
                           Consolpp::Stream::ErrorOutputStream& errorOutput)
            {
                std::size_t number = 0;

                errorOutput.write("<comment>The output is {} and {}.</comment>\n",
                                  output.isTerminal() ? "a terminal" : "no terminal",
                                  output.isDecorated() ? "decorated" : "not decorated");

                for (const std::string_view line : input.createLineRange()) {
                    ++number;
                    output.write("<info>{:>4}</info> {}\n", number, line);
                }

                return 0;
            }
    };
} // namespace Demo::Output
