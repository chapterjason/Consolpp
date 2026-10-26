#pragma once

#include "Consolpp/Formatting/FormatValuePlaceholder.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"

#include <algorithm>
#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    template <std::output_iterator<char> Output>
    constexpr Output formatOptionLabel(const Option& option, const bool negatable, Output output)
    {
        const auto write = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto put = [&output](const char character) {
            *output = character;
            ++output;
        };

        if (option.shortcut.has_value()) {
            put('-');
            put(*option.shortcut);
            write(", ");
        } else {
            write("    ");
        }

        write("--");
        write(option.name);

        if (option.value == OptionValue::None) {
            if (negatable) {
                write("|--no-");
                write(option.name);
            }

            return output;
        }

        write(option.value == OptionValue::Optional ? "[=" : "=");

        output = Formatting::formatValuePlaceholder(option.name, output);

        if (option.value == OptionValue::Optional) {
            put(']');
        }

        return output;
    }
} // namespace Consolpp::Formatting
