#pragma once

#include "Consolpp/Markup/Encode.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>
#include <span>
#include <string_view>

namespace Consolpp::Formatting
{
    // Encoded markup of the default value or the comma separated default values.
    template <std::output_iterator<char> Output>
    constexpr Output formatDefaultAsText(const std::optional<std::string_view> defaultValue,
                                         const std::span<const std::string_view> defaultValues,
                                         Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };

        copy("<comment> [default: ");

        if (defaultValue.has_value()) {
            output = Markup::encode(*defaultValue, output);
        }

        for (std::size_t index = 0; const std::string_view value : defaultValues) {
            copy(index == 0 ? "" : ", ");
            output = Markup::encode(value, output);
            ++index;
        }

        copy("]</comment>");

        return output;
    }
} // namespace Consolpp::Formatting
