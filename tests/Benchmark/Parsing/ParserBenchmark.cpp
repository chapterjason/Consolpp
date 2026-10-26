#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/ParseError.hpp"
#include "Consolpp/Parsing/ParseErrorKind.hpp"
#include "Consolpp/Parsing/Parser.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Signature.hpp"

#include <array>
#include <string_view>

#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <benchmark/utils.h>

namespace Consolpp::Tests::Benchmark::Parsing
{
    namespace
    {
        constexpr Signature signature{
            std::array{
                Option{.name = "all", .shortcut = 'a'},
                Option{.name = "force", .shortcut = 'f'},
                Option{.name = "color"},
                Option{
                    .name = "output",
                    .shortcut = 'o',
                    .value = OptionValue::Required,
                },
                Option{
                    .name = "environment",
                    .shortcut = 'e',
                    .value = OptionValue::Required,
                },
                Option{.name = "tag", .shortcut = 't', .value = OptionValue::Optional},
                Option{
                    .name = "include",
                    .shortcut = 'i',
                    .value = OptionValue::Required,
                    .repeatable = true,
                },
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "target", .required = false},
            },
        };

        constexpr Definition definition{.name = "deploy", .signature = signature};

        using Parser = Consolpp::Parsing::Parser<definition>;
        using Parameters = Consolpp::Parsing::Parameters<definition>;

        constexpr Signature variadicSignature{
            std::array{
                Option{.name = "force", .shortcut = 'f'},
                Option{
                    .name = "output",
                    .shortcut = 'o',
                    .value = OptionValue::Required,
                },
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "files", .variadic = true},
            },
        };

        constexpr Definition variadicDefinition{.name = "deploy", .signature = variadicSignature};

        using VariadicParser = Consolpp::Parsing::Parser<variadicDefinition>;
        using VariadicParameters = Consolpp::Parsing::Parameters<variadicDefinition>;

        void parseMixedArguments(benchmark::State& state)
        {
            // Arrange
            std::array arguments{
                "a.txt",
                "--environment=production",
                "-af",
                "-o",
                "out.log",
                "--tag=stable",
                "--no-color",
                "b.txt",
            };
            const Consolpp::Parsing::TokenRange tokens{arguments};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(arguments);

                Parser parser{tokens};
                Parameters parameters = parser.parse();

                benchmark::DoNotOptimize(parameters);
            }
        }

        void parseArgumentsOnly(benchmark::State& state)
        {
            // Arrange
            std::array arguments{"a.txt", "b.txt"};
            const Consolpp::Parsing::TokenRange tokens{arguments};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(arguments);

                Parser parser{tokens};
                Parameters parameters = parser.parse();

                benchmark::DoNotOptimize(parameters);
            }
        }

        void parseRepeatableOption(benchmark::State& state)
        {
            // Arrange
            std::array arguments{"a.txt", "--include=a", "-i", "b", "-ic", "--include", "d"};
            const Consolpp::Parsing::TokenRange tokens{arguments};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(arguments);

                Parser parser{tokens};
                const Parameters parameters = parser.parse();

                for (std::string_view value : parameters.getOption<"include">()) {
                    benchmark::DoNotOptimize(value);
                }
            }
        }

        void parseVariadicArgument(benchmark::State& state)
        {
            // Arrange
            std::array arguments{"a.txt", "b.txt", "-f", "c.txt", "-o", "out.txt", "d.txt", "e.txt"};
            const Consolpp::Parsing::TokenRange tokens{arguments};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(arguments);

                VariadicParser parser{tokens};
                const VariadicParameters parameters = parser.parse();

                for (std::string_view value : parameters.getArgument<"files">()) {
                    benchmark::DoNotOptimize(value);
                }
            }
        }

        void parseUnknownOption(benchmark::State& state)
        {
            // Arrange
            std::array arguments{"a.txt", "--bogus"};
            const Consolpp::Parsing::TokenRange tokens{arguments};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                benchmark::DoNotOptimize(arguments);

                Parser parser{tokens};

                try {
                    Parameters parameters = parser.parse();

                    benchmark::DoNotOptimize(parameters);
                } catch (const Consolpp::Parsing::ParseError& error) {
                    Consolpp::Parsing::ParseErrorKind kind = error.getKind();

                    benchmark::DoNotOptimize(kind);
                }
            }
        }
    } // namespace

    BENCHMARK(parseMixedArguments);
    BENCHMARK(parseArgumentsOnly);
    BENCHMARK(parseRepeatableOption);
    BENCHMARK(parseVariadicArgument);
    BENCHMARK(parseUnknownOption);
} // namespace Consolpp::Tests::Benchmark::Parsing
