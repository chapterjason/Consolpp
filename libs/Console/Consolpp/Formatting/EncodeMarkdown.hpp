#pragma once

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    // CommonMark backslash escapes keep the text literal, also the list markers at the start of a line.
    template <std::output_iterator<char> Output>
    constexpr Output encodeMarkdown(const std::string_view text, Output output)
    {
        constexpr std::string_view special = "\\`*_[]<>#|~";
        std::size_t lineStart = 0;

        for (std::size_t index = 0; index < text.size(); ++index) {
            const char character = text.at(index);
            const std::string_view before = text.substr(lineStart, index - lineStart);
            const bool bulletMarker = before.empty() && (character == '-' || character == '+' || character == '=');
            const bool orderedMarker = !before.empty()
                                       && before.find_first_not_of("0123456789") == std::string_view::npos
                                       && (character == '.' || character == ')');

            if (bulletMarker || orderedMarker || special.find(character) != std::string_view::npos) {
                *output = '\\';
                ++output;
            }

            *output = character;
            ++output;

            if (character == '\n') {
                lineStart = index + 1;
            }
        }

        return output;
    }
} // namespace Consolpp::Formatting
