#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <optional>
#include <string_view>

namespace Consolpp::Formatting
{
    // A JSON string (RFC 8259) with only the escapes JSON requires; other characters stay as they are.
    template <std::output_iterator<char> Output>
    constexpr Output encodeJsonString(const std::string_view text, Output output)
    {
        constexpr std::array<char, 16>
            hexDigits{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        constexpr unsigned char firstPrintable = 0x20;
        constexpr unsigned char nibble = 4;
        constexpr unsigned char lowNibble = 0x0F;
        const auto write = [&output](const std::string_view escaped) {
            output = std::ranges::copy(escaped, output).out;
        };
        const auto put = [&output](const char character) {
            *output = character;
            ++output;
        };

        put('"');

        for (const char character : text) {
            const auto byte = static_cast<unsigned char>(character);

            switch (character) {
                case '"':
                    write("\\\"");
                    break;
                case '\\':
                    write("\\\\");
                    break;
                case '\b':
                    write("\\b");
                    break;
                case '\f':
                    write("\\f");
                    break;
                case '\n':
                    write("\\n");
                    break;
                case '\r':
                    write("\\r");
                    break;
                case '\t':
                    write("\\t");
                    break;
                default:
                    if (byte < firstPrintable) {
                        write("\\u00");
                        put(hexDigits.at(byte >> nibble));
                        put(hexDigits.at(byte & lowNibble));
                    } else {
                        put(character);
                    }
            }
        }

        put('"');

        return output;
    }

    // A missing value is null.
    template <std::output_iterator<char> Output>
    constexpr Output encodeJsonString(const std::optional<std::string_view> text, Output output)
    {
        if (!text.has_value()) {
            return std::ranges::copy(std::string_view{"null"}, output).out;
        }

        return Formatting::encodeJsonString(*text, output);
    }
} // namespace Consolpp::Formatting
