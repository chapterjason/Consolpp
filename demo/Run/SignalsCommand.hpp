#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Event/Registration.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>

// SIGINT and SIGWINCH are POSIX and not all declared through <csignal>.
#include <signal.h> // NOLINT(modernize-deprecated-headers)

namespace Demo::Run
{
    class SignalsCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "run:signals",
                .description = "Reacts to Ctrl+C and to resizing the terminal",
                .help = "The first Ctrl+C asks for a second one, which ends the command.",
                .signature = Consolpp::Signature{std::array<Consolpp::Option, 0>{},
                                                 std::array<Consolpp::Argument, 0>{}},
            };

            static int run(Consolpp::Stream::OutputStream& output, Consolpp::Event::Loop& loop)
            {
                bool interrupted = false;

                output.write("<info>Waiting.</info> Resize the terminal or press Ctrl+C.\n");
                output.flush();

                const Consolpp::Event::Registration interrupt = loop.registerSignal(
                    SIGINT,
                    [&output, &loop, &interrupted] {
                        if (interrupted) {
                            output.write("\n<comment>Interrupted again, stopping.</comment>\n");
                            loop.stop();

                            return;
                        }

                        interrupted = true;
                        output.write("\n<comment>Interrupted.</comment> Press Ctrl+C again to stop.\n");
                        output.flush();
                    });
                const Consolpp::Event::Registration resize = loop.registerSignal(SIGWINCH, [&output] {
                    output.write("Resized to <info>{}</info> columns.\n", output.getColumns());
                    output.flush();
                });

                loop.run();

                return 0;
            }
    };
} // namespace Demo::Run
