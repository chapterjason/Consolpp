#pragma once

#include "Consolpp/Formatting/EncodeRoff.hpp"
#include "Consolpp/Formatting/FormatValuePlaceholder.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // A tagged paragraph with the shortcut, the name with its value, what is special about the option and its
    // stripped description markup.
    template <std::output_iterator<char> Output>
    Output formatOptionAsManPage(const Option& option, const bool negatable, Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto roff = [&output](const std::string_view text) {
            output = Formatting::encodeRoff(text, output);
        };
        std::string placeholder;
        std::string description;

        Formatting::formatValuePlaceholder(option.name, std::back_inserter(placeholder));
        Styling::stripMarkup(option.description, std::back_inserter(description));
        copy(".TP\n");

        if (option.shortcut.has_value()) {
            copy("\\fB\\-");
            roff(std::string_view{&*option.shortcut, 1});
            copy("\\fR, ");
        }

        copy(R"(\fB\-\-)");
        roff(option.name);
        copy("\\fR");

        if (option.value == OptionValue::Required) {
            copy("=\\fI");
            roff(placeholder);
            copy("\\fR");
        } else if (option.value == OptionValue::Optional) {
            copy("[=\\fI");
            roff(placeholder);
            copy("\\fR]");
        }

        if (negatable && option.value == OptionValue::None) {
            copy(R"(, \fB\-\-no\-)");
            roff(option.name);
            copy("\\fR");
        }

        copy("\n");
        roff(description);

        if (option.defaultValue.has_value()) {
            copy(" (default: ");
            roff(*option.defaultValue);
            copy(")");
        }

        for (bool first = true; const std::string_view value : option.defaultValues) {
            copy(first ? " (default: " : ", ");
            roff(value);
            first = false;
        }

        copy(option.defaultValues.empty() ? "" : ")");
        copy(option.repeatable ? " (multiple values allowed)" : "");
        copy("\n");

        return output;
    }
} // namespace Consolpp::Formatting
