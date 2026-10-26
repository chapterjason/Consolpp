#pragma once

#include <algorithm>
#include <compare>
#include <cstddef>
#include <string_view>
#include <utility>

namespace Consolpp::Text
{
    constexpr std::weak_ordering compareNaturalIgnoringCase(const std::string_view left, const std::string_view right)
    {
        const auto isDigit = [](const char character) {
            return character >= '0' && character <= '9';
        };
        const auto toLower = [](const char character) {
            return character >= 'A' && character <= 'Z' ? static_cast<char>(character - 'A' + 'a') : character;
        };
        // The digit run at the start of the text without its leading zeros, and the length of the whole run.
        const auto parseNumber = [](const std::string_view text) {
            const std::size_t length = std::min(text.find_first_not_of("0123456789"), text.size());
            const std::size_t zeros = std::min(text.find_first_not_of('0'), length);

            return std::pair{text.substr(zeros, length - zeros), length};
        };
        std::size_t leftIndex = 0;
        std::size_t rightIndex = 0;

        while (leftIndex < left.size() && rightIndex < right.size()) {
            if (isDigit(left.at(leftIndex)) && isDigit(right.at(rightIndex))) {
                const auto [leftNumber, leftLength] = parseNumber(left.substr(leftIndex));
                const auto [rightNumber, rightLength] = parseNumber(right.substr(rightIndex));

                if (leftNumber.size() != rightNumber.size()) {
                    return leftNumber.size() <=> rightNumber.size();
                }

                if (leftNumber != rightNumber) {
                    return leftNumber <=> rightNumber;
                }

                leftIndex += leftLength;
                rightIndex += rightLength;

                continue;
            }

            const char leftCharacter = toLower(left.at(leftIndex));
            const char rightCharacter = toLower(right.at(rightIndex));

            if (leftCharacter != rightCharacter) {
                return static_cast<unsigned char>(leftCharacter) <=> static_cast<unsigned char>(rightCharacter);
            }

            ++leftIndex;
            ++rightIndex;
        }

        return (left.size() - leftIndex) <=> (right.size() - rightIndex);
    }
} // namespace Consolpp::Text
