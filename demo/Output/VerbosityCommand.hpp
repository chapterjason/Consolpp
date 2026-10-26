#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Stream/Verbosity.hpp"

#include <array>
#include <string_view>

namespace Demo::Output
{
    class VerbosityCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "output:verbosity",
                .description = "Writes a message at every verbosity level",
                .usages =
                    std::array<std::string_view, 5>{
                        "output:verbosity --silent",
                        "output:verbosity -q",
                        "output:verbosity -v",
                        "output:verbosity -vv",
                        "output:verbosity -vvv",
                    },
                .signature = Consolpp::Signature{std::array<Consolpp::Option, 0>{},
                                                 std::array<Consolpp::Argument, 0>{}},
            };

            static int run(Consolpp::Stream::OutputStream& output, Consolpp::Stream::ErrorOutputStream& errorOutput)
            {
                using Consolpp::Stream::Verbosity;

                errorOutput.write(Verbosity::Quiet, "<error>quiet</error>        an error, shown unless --silent\n");
                output.write(Verbosity::Normal, "<info>normal</info>       shown without -q\n");
                output.write(Verbosity::Verbose, "<info>verbose</info>      shown with -v\n");
                output.write(Verbosity::VeryVerbose, "<info>very verbose</info> shown with -vv\n");
                output.write(Verbosity::Debug, "<info>debug</info>        shown with -vvv\n");

                return 0;
            }
    };
} // namespace Demo::Output
