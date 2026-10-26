#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/FormatOptionSynopsis.hpp"
#include "Consolpp/Formatting/SynopsisForm.hpp"
#include "Consolpp/GlobalOptions.hpp"
#include "Consolpp/Option.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    template <std::output_iterator<char> Output>
    constexpr Output formatSynopsis(const DefinitionView& command, const SynopsisForm form, Output output)
    {
        const auto write = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto put = [&output](const char character) {
            *output = character;
            ++output;
        };
        std::size_t optionalArguments = 0;

        write(command.name);
        if (form == SynopsisForm::Short) {
            write(" [options]");
        } else {
            put(' ');
            output = Formatting::formatOptionSynopsis(globalOptions.front(), false, output);

            for (const Option& option : command.options) {
                put(' ');
                output = Formatting::formatOptionSynopsis(option, true, output);
            }
        }

        if (!command.arguments.empty()) {
            write(" [--]");
        }

        for (const Argument& argument : command.arguments) {
            write(argument.required ? " <" : " [<");
            write(argument.name);
            write(argument.variadic ? ">..." : ">");

            if (!argument.required) {
                ++optionalArguments;
            }
        }

        for (std::size_t index = 0; index < optionalArguments; ++index) {
            put(']');
        }

        return output;
    }
} // namespace Consolpp::Formatting
