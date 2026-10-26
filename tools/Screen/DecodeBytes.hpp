#pragma once

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Consolpp::Tools::Screen
{
    // Reverses encodeBytes(). Throws std::invalid_argument for a backslash that does not start \xHH.
    constexpr std::back_insert_iterator<std::string> decodeBytes(const std::string_view text,
                                                                 std::back_insert_iterator<std::string> output)
    {
        constexpr std::size_t escapeLength = 4;
        constexpr int hexBase = 16;
        constexpr int firstLetterValue = 10;
        const auto hexValue = [](const char digit) {
            if (digit >= '0' && digit <= '9') {
                return digit - '0';
            }

            if (digit >= 'a' && digit <= 'f') {
                return digit - 'a' + firstLetterValue;
            }

            throw std::invalid_argument{"An escape must have two lowercase hex digits"};
        };

        for (std::size_t index = 0; index < text.size();) {
            if (text.at(index) != '\\') {
                *output = text.at(index);
                ++output;
                ++index;

                continue;
            }

            if (index + escapeLength > text.size() || text.at(index + 1) != 'x') {
                throw std::invalid_argument{"A backslash must start an escape like \\x1b"};
            }

            *output = static_cast<char>((hexValue(text.at(index + 2)) * hexBase) + hexValue(text.at(index + 3)));
            ++output;
            index += escapeLength;
        }

        return output;
    }
} // namespace Consolpp::Tools::Screen
