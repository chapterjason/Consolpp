#include "Consolpp/Argument.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Signature.hpp"

#include <array>

#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <benchmark/utils.h>

namespace Consolpp::Tests::Benchmark
{
    namespace
    {
        constexpr Consolpp::Signature signature{
            std::array{
                Consolpp::Option{.name = "all", .shortcut = 'a'},
                Consolpp::Option{
                    .name = "output",
                    .shortcut = 'o',
                    .value = Consolpp::OptionValue::Required,
                },
                Consolpp::Option{
                    .name = "tag",
                    .value = Consolpp::OptionValue::Optional,
                    .repeatable = true,
                },
                Consolpp::Option{.name = "force"},
            },
            std::array{
                Consolpp::Argument{.name = "source"},
                Consolpp::Argument{.name = "target", .required = false},
                Consolpp::Argument{.name = "extra", .required = false, .variadic = true},
            },
        };

        void iterateOptions(benchmark::State& state)
        {
            // Arrange
            auto copy = signature;

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(copy);

                for (Consolpp::Option option : copy.getOptions()) {
                    benchmark::DoNotOptimize(option);
                }
            }
        }

        void iterateArguments(benchmark::State& state)
        {
            // Arrange
            auto copy = signature;

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(copy);

                for (Consolpp::Argument argument : copy.getArguments()) {
                    benchmark::DoNotOptimize(argument);
                }
            }
        }
    } // namespace

    BENCHMARK(iterateOptions);
    BENCHMARK(iterateArguments);
} // namespace Consolpp::Tests::Benchmark
