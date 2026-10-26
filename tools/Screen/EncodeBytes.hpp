#pragma once

#include <array>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Tools::Screen
{
    // Control bytes and backslashes become \xHH, so an entry fits on one line; the rest stays readable.
    constexpr std::back_insert_iterator<std::string> encodeBytes(const std::string_view bytes,
                                                                 std::back_insert_iterator<std::string> output)
    {
        constexpr std::string_view hexDigits = "0123456789abcdef";
        constexpr unsigned char firstPrintable = 0x20;
        constexpr unsigned char deleteByte = 0x7f;
        constexpr unsigned char hexBase = 16;

        for (const char character : bytes) {
            const auto byte = static_cast<unsigned char>(character);

            if (byte >= firstPrintable && byte != deleteByte && character != '\\') {
                *output = character;
                ++output;

                continue;
            }

            for (const char part :
                 std::array<char, 4>{'\\', 'x', hexDigits.at(byte / hexBase), hexDigits.at(byte % hexBase)}) {
                *output = part;
                ++output;
            }
        }

        return output;
    }
} // namespace Consolpp::Tools::Screen
