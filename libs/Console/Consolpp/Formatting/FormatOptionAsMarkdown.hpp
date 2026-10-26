#pragma once

#include "Consolpp/Formatting/EncodeMarkdownCode.hpp"
#include "Consolpp/Formatting/FormatMarkdownDefaults.hpp"
#include "Consolpp/Formatting/FormatMarkdownDescription.hpp"
#include "Consolpp/Formatting/FormatValuePlaceholder.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // A list item with the shortcut, the name with its value, what is special about the option and its description.
    template <std::output_iterator<char> Output>
    Output formatOptionAsMarkdown(const Option& option, const bool negatable, Output output)
    {
        std::string name = "--" + std::string{option.name};
        bool annotated = false;
        const auto annotate = [&output, &annotated](const std::string_view text) {
            output = std::ranges::copy(std::string_view{annotated ? ", " : " ("}, output).out;
            output = std::ranges::copy(text, output).out;
            annotated = true;
        };

        if (option.value != OptionValue::None) {
            name += option.value == OptionValue::Optional ? "[=" : "=";
            Formatting::formatValuePlaceholder(option.name, std::back_inserter(name));
            name += option.value == OptionValue::Optional ? "]" : "";
        }

        output = std::ranges::copy(std::string_view{"- "}, output).out;

        if (option.shortcut.has_value()) {
            output = Formatting::encodeMarkdownCode(std::string{'-', *option.shortcut}, output);
            output = std::ranges::copy(std::string_view{", "}, output).out;
        }

        output = Formatting::encodeMarkdownCode(name, output);

        if (option.repeatable) {
            annotate("multiple");
        }

        if (option.value == OptionValue::None && option.defaultEnabled.value_or(false)) {
            annotate("enabled by default");
        }

        if (option.value != OptionValue::None) {
            output = Formatting::formatMarkdownDefaults(option.defaultValue, option.defaultValues, annotated, output);
        }

        if (negatable && option.value == OptionValue::None) {
            annotate("negatable with ");
            output = Formatting::encodeMarkdownCode("--no-" + std::string{option.name}, output);
        }

        output = std::ranges::copy(std::string_view{annotated ? ")" : ""}, output).out;

        return Formatting::formatMarkdownDescription(option.description, output);
    }
} // namespace Consolpp::Formatting
