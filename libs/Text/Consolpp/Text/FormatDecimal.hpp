#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>

namespace Consolpp::Text
{
    template <std::output_iterator<char> Output>
    constexpr Output formatDecimal(std::uint32_t value, Output output)
    {
        constexpr std::uint32_t base = 10;
        constexpr std::size_t maxDigits = std::numeric_limits<std::uint32_t>::digits10 + 1;
        std::array<char, maxDigits> digits{};
        std::size_t count = 0;

        while (count == 0 || value != 0) {
            digits.at(count) = static_cast<char>('0' + (value % base));
            value /= base;
            ++count;
        }

        while (count > 0) {
            --count;
            *output = digits.at(count);
            ++output;
        }

        return output;
    }
} // namespace Consolpp::Text
