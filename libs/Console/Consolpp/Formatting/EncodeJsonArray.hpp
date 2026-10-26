#pragma once

#include "Consolpp/Formatting/EncodeJsonString.hpp"

#include <iterator>
#include <span>
#include <string_view>

namespace Consolpp::Formatting
{
    template <std::output_iterator<char> Output>
    constexpr Output encodeJsonArray(const std::span<const std::string_view> values, Output output)
    {
        bool first = true;

        *output = '[';
        ++output;

        for (const std::string_view value : values) {
            if (!first) {
                *output = ',';
                ++output;
            }

            first = false;
            output = Formatting::encodeJsonString(value, output);
        }

        *output = ']';
        ++output;

        return output;
    }
} // namespace Consolpp::Formatting
