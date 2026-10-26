#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Formatting/EncodeMarkdownCode.hpp"
#include "Consolpp/Formatting/FormatMarkdownDefaults.hpp"
#include "Consolpp/Formatting/FormatMarkdownDescription.hpp"

#include <algorithm>
#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    // A list item with the name, what is special about the argument and its description.
    template <std::output_iterator<char> Output>
    Output formatArgumentAsMarkdown(const Argument& argument, Output output)
    {
        bool annotated = false;
        const auto annotate = [&output, &annotated](const std::string_view text) {
            output = std::ranges::copy(std::string_view{annotated ? ", " : " ("}, output).out;
            output = std::ranges::copy(text, output).out;
            annotated = true;
        };

        output = std::ranges::copy(std::string_view{"- "}, output).out;
        output = Formatting::encodeMarkdownCode(argument.name, output);

        if (!argument.required) {
            annotate("optional");
        }

        if (argument.variadic) {
            annotate("multiple");
        }

        output = Formatting::formatMarkdownDefaults(argument.defaultValue, argument.defaultValues, annotated, output);
        output = std::ranges::copy(std::string_view{annotated ? ")" : ""}, output).out;

        return Formatting::formatMarkdownDescription(argument.description, output);
    }
} // namespace Consolpp::Formatting
