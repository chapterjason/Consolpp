#pragma once

#include <algorithm>
#include <iterator>
#include <string_view>

namespace Consolpp::Markup
{
    // The result is safe as markup text and as an attribute value.
    template <std::output_iterator<char> Output>
    constexpr Output encode(const std::string_view text, Output output)
    {
        const auto copy = [&output](const std::string_view entity) {
            output = std::ranges::copy(entity, output).out;
        };

        for (const char character : text) {
            switch (character) {
                case '<':
                    copy("&lt;");
                    break;
                case '>':
                    copy("&gt;");
                    break;
                case '&':
                    copy("&amp;");
                    break;
                case '"':
                    copy("&quot;");
                    break;
                case '\'':
                    copy("&apos;");
                    break;
                default:
                    *output = character;
                    ++output;
            }
        }

        return output;
    }
} // namespace Consolpp::Markup
