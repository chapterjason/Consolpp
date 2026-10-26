#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Formatting/FormatDefaultAsText.hpp"
#include "Consolpp/Formatting/FormatDescriptionAsText.hpp"
#include "Consolpp/Formatting/FormatOptionLabel.hpp"
#include "Consolpp/GlobalOptions.hpp"
#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <span>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // Markup; the descriptions are markup themselves, every other text is encoded.
    template <std::output_iterator<char> Output>
    Output formatSignatureAsText(const std::span<const Option> options,
                                 const std::span<const Argument> arguments,
                                 Output output)
    {
        std::string label;
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto encode = [&output](const std::string_view text) {
            output = Markup::encode(text, output);
        };
        const auto pad = [&output](const std::size_t count) {
            output = std::ranges::fill_n(output, static_cast<std::ptrdiff_t>(count), ' ');
        };
        const auto formatLabel = [&label](const Option& option, const bool negatable) -> std::string_view {
            label.clear();
            Formatting::formatOptionLabel(option, negatable, std::back_inserter(label));

            return label;
        };
        std::size_t columns = 0;

        for (const Option& option : globalOptions) {
            columns = std::max(columns, Text::measureDisplayWidth(formatLabel(option, false)));
        }

        for (const Option& option : options) {
            columns = std::max(columns, Text::measureDisplayWidth(formatLabel(option, true)));
        }

        for (const Argument& argument : arguments) {
            columns = std::max(columns, Text::measureDisplayWidth(argument.name));
        }

        const auto formatRow = [&copy, &encode, &pad, &output, columns](const std::string_view rowLabel,
                                                                        const std::string_view description) {
            copy("  <info>");
            encode(rowLabel);
            copy("</info>  ");
            pad(columns - Text::measureDisplayWidth(rowLabel));
            output = Formatting::formatDescriptionAsText(description, columns + 4, output);
        };

        if (!arguments.empty()) {
            copy("<comment>Arguments:</comment>\n");

            for (const Argument& argument : arguments) {
                formatRow(argument.name, argument.description);

                if (argument.defaultValue.has_value() || !argument.defaultValues.empty()) {
                    output = Formatting::formatDefaultAsText(argument.defaultValue, argument.defaultValues, output);
                }

                copy("\n");
            }

            copy("\n");
        }

        copy("<comment>Options:</comment>\n");

        for (bool first = true; const Option& option : globalOptions) {
            if (!first) {
                copy("\n");
            }

            formatRow(formatLabel(option, false), option.description);
            first = false;
        }

        for (const Option& option : options) {
            const bool takesValue = option.value != OptionValue::None;

            copy("\n");
            formatRow(formatLabel(option, true), option.description);

            if (takesValue && (option.defaultValue.has_value() || !option.defaultValues.empty())) {
                output = Formatting::formatDefaultAsText(option.defaultValue, option.defaultValues, output);
            }

            if (option.repeatable) {
                copy("<comment> (multiple values allowed)</comment>");
            }
        }

        return output;
    }
} // namespace Consolpp::Formatting
