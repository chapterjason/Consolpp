#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <array>
#include <span>

#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <benchmark/utils.h>

namespace Consolpp::Tests::Benchmark::Parsing
{
    namespace
    {
        void iterateMixedArguments(benchmark::State& state)
        {
            // Arrange
            std::array arguments{
                "deploy",
                "--environment=production",
                "-vvv",
                "--force",
                "-o",
                "output.log",
                "--tag=a=b",
                "-",
                "--",
                "extra",
            };
            const Consolpp::Parsing::TokenRange tokens{arguments};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(arguments);

                for (Consolpp::Parsing::Token token : tokens) {
                    benchmark::DoNotOptimize(token);
                }
            }
        }

        void dereference(benchmark::State& state, const char* const argument)
        {
            // Arrange
            std::array arguments{argument};
            const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(arguments);

                Consolpp::Parsing::Token token = *iterator;

                benchmark::DoNotOptimize(token);
            }
        }
    } // namespace

    BENCHMARK(iterateMixedArguments);
    BENCHMARK_CAPTURE(dereference, dash, "-");
    BENCHMARK_CAPTURE(dereference, positional, "output.log");
    BENCHMARK_CAPTURE(dereference, shortOptions, "-vvv");
    BENCHMARK_CAPTURE(dereference, separator, "--");
    BENCHMARK_CAPTURE(dereference, longOption, "--force");
    BENCHMARK_CAPTURE(dereference, longOptionWithValue, "--environment=production");
    BENCHMARK_CAPTURE(dereference, longOptionWithShortValue, "--force=x");
} // namespace Consolpp::Tests::Benchmark::Parsing
