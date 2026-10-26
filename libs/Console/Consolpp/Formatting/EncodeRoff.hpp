#pragma once

#include <algorithm>
#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    // Text for a man page: backslashes and dashes are escaped, and a line starting with "." or "'", which roff would
    // read as a request, is protected.
    template <std::output_iterator<char> Output>
    constexpr Output encodeRoff(const std::string_view text, Output output)
    {
        bool lineStart = true;

        for (const char character : text) {
            if (lineStart && (character == '.' || character == '\'')) {
                output = std::ranges::copy(std::string_view{"\\&"}, output).out;
            }

            if (character == '\\') {
                output = std::ranges::copy(std::string_view{"\\e"}, output).out;
            } else if (character == '-') {
                output = std::ranges::copy(std::string_view{"\\-"}, output).out;
            } else {
                *output = character;
                ++output;
            }

            lineStart = character == '\n';
        }

        return output;
    }
} // namespace Consolpp::Formatting
