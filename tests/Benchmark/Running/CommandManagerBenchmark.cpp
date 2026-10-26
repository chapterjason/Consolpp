#include "Consolpp/Argument.hpp"
#include "Consolpp/CommandManager.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <cerrno>
#include <cstddef>
#include <system_error>
#include <tuple>
#include <utility>

#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <benchmark/utils.h>
#include <fcntl.h>
#include <unistd.h>

namespace Consolpp::Tests::Benchmark::Running
{
    using Consolpp::Parsing::Parameters;

    namespace
    {
        constexpr std::array<const char*, 16> names{
            "alpha",
            "bravo",
            "charlie",
            "delta",
            "echo",
            "foxtrot",
            "golf",
            "hotel",
            "india",
            "juliett",
            "kilo",
            "lima",
            "mike",
            "november",
            "oscar",
            "papa",
        };

        template <std::size_t Id>
        class FakeCommand
        {
            private:
                int forcedExitCode{1};

            public:
                static constexpr Definition definition{
                    .name = names.at(Id),
                    .signature =
                        Signature{
                            std::array{Option{.name = "force", .shortcut = 'f'}},
                            std::array{Argument{.name = "source"}},
                        },
                };

                [[nodiscard]]
                int run(const Parameters<definition>& parameters) const
                {
                    return parameters.template getOption<"force">().value_or(false)
                               ? this->forcedExitCode
                               : static_cast<int>(parameters.template getArgument<"source">().size());
                }
        };

        template <typename Manager>
        Manager addCommands(const Manager& manager)
        {
            return manager;
        }

        template <typename Manager, typename First, typename... Rest>
        auto addCommands(const Manager& manager, First& first, Rest&... rest)
        {
            return addCommands(manager.addCommand(first), rest...);
        }

        template <std::size_t... Ids>
        void runLastCommand(benchmark::State& state, std::index_sequence<Ids...> /*ids*/)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Event::Loop loop;
            Consolpp::Stream::InputStream input{-1};
            Consolpp::Stream::OutputStream output{descriptor,
                                                  Consolpp::Stream::FlushMode::Buffered,
                                                  false,
                                                  Terminal::Capabilities{}};
            Consolpp::Stream::ErrorOutputStream errorOutput{descriptor,
                                                            Consolpp::Stream::FlushMode::Buffered,
                                                            false,
                                                            Terminal::Capabilities{}};
            std::tuple<FakeCommand<Ids>...> commands;
            std::array<const char*, 3> arguments{names.at(sizeof...(Ids) - 1), "a.txt", "--force"};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(arguments);

                const auto manager = addCommands(CommandManager{input, output, errorOutput, loop},
                                                 std::get<Ids>(commands)...);
                int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

                benchmark::DoNotOptimize(exitCode);
            }

            if (close(descriptor) != 0) {
                throw std::system_error{errno, std::generic_category()};
            }
        }
    } // namespace

    BENCHMARK_CAPTURE(runLastCommand, three, std::make_index_sequence<3>{});
    BENCHMARK_CAPTURE(runLastCommand, sixteen, std::make_index_sequence<16>{});
} // namespace Consolpp::Tests::Benchmark::Running
