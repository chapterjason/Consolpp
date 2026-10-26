#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"

#include <array>
#include <charconv>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string_view>
#include <system_error>

namespace Demo::Run
{
    inline constexpr std::array<std::string_view, 3> exitCodes{"0", "1", "2"};

    class ExitCodeCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "run:exit-code",
                .description = "Ends with the given exit code",
                .usages = std::array<std::string_view, 1>{"run:exit-code 3; echo $?"},
                .signature =
                    Consolpp::Signature{
                        std::array<Consolpp::Option, 0>{},
                        std::array{
                            Consolpp::Argument{
                                .name = "code",
                                .description = "The exit code, from 0 to 255",
                                .suggestedValues = exitCodes,
                            },
                        },
                    },
            };

            // Throws std::invalid_argument for a code that is no number from 0 to 255.
            static int run(const Consolpp::Parsing::Parameters<definition>& parameters)
            {
                constexpr int highestCode = 255;
                const std::string_view text = parameters.getArgument<"code">();
                const char* const end = std::next(text.data(), static_cast<std::ptrdiff_t>(text.size()));
                int code = 0;
                const std::from_chars_result parsed = std::from_chars(text.data(), end, code);

                if (parsed.ec != std::errc{} || parsed.ptr != end || code < 0 || code > highestCode) {
                    throw std::invalid_argument{"The exit code has to be a number from 0 to 255"};
                }

                return code;
            }
    };
} // namespace Demo::Run
