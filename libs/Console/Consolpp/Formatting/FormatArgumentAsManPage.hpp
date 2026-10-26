#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Formatting/EncodeRoff.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // A tagged paragraph with the name, what is special about the argument and its stripped description markup.
    template <std::output_iterator<char> Output>
    Output formatArgumentAsManPage(const Argument& argument, Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto roff = [&output](const std::string_view text) {
            output = Formatting::encodeRoff(text, output);
        };
        std::string description;

        Styling::stripMarkup(argument.description, std::back_inserter(description));
        copy(".TP\n\\fI");
        roff(argument.name);
        copy("\\fR");
        copy(argument.required ? "" : " (optional)");
        copy(argument.variadic ? " (multiple)" : "");
        copy("\n");
        roff(description);

        if (argument.defaultValue.has_value()) {
            copy(" (default: ");
            roff(*argument.defaultValue);
            copy(")");
        }

        for (bool first = true; const std::string_view value : argument.defaultValues) {
            copy(first ? " (default: " : ", ");
            roff(value);
            first = false;
        }

        copy(argument.defaultValues.empty() ? "" : ")");
        copy("\n");

        return output;
    }
} // namespace Consolpp::Formatting
