#pragma once

#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    template <std::output_iterator<char> Output>
    constexpr Output formatValuePlaceholder(const std::string_view optionName, Output output)
    {
        for (const char character : optionName) {
            *output = character >= 'a' && character <= 'z' ? static_cast<char>(character - 'a' + 'A') : character;
            ++output;
        }

        return output;
    }
} // namespace Consolpp::Formatting
