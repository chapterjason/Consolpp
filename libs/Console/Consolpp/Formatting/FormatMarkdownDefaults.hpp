#pragma once

#include "Consolpp/Formatting/EncodeMarkdownCode.hpp"

#include <algorithm>
#include <iterator>
#include <optional>
#include <span>
#include <string_view>

namespace Consolpp::Formatting
{
    // Adds "default `a`, `b`" to the annotations of a list item; annotated tells whether it has one already.
    template <std::output_iterator<char> Output>
    constexpr Output formatMarkdownDefaults(const std::optional<std::string_view> defaultValue,
                                            const std::span<const std::string_view> defaultValues,
                                            bool& annotated,
                                            Output output)
    {
        if (!defaultValue.has_value() && defaultValues.empty()) {
            return output;
        }

        output = std::ranges::copy(std::string_view{annotated ? ", default " : " (default "}, output).out;
        annotated = true;

        if (defaultValue.has_value()) {
            output = Formatting::encodeMarkdownCode(*defaultValue, output);
        }

        for (bool first = true; const std::string_view value : defaultValues) {
            output = std::ranges::copy(std::string_view{first ? "" : ", "}, output).out;
            output = Formatting::encodeMarkdownCode(value, output);
            first = false;
        }

        return output;
    }
} // namespace Consolpp::Formatting
