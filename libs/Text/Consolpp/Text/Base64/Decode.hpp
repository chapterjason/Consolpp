#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Text::Base64
{
    // RFC 4648 Base64, with or without padding.
    template <std::output_iterator<char> Output>
    constexpr Output decode(const std::string_view text, Output output)
    {
        constexpr std::uint32_t bitsPerCharacter = 6;
        constexpr std::uint32_t bitsPerByte = 8;
        constexpr std::uint32_t byteMask = 0xFF;
        constexpr std::size_t charactersPerGroup = 4;
        constexpr std::size_t paddingLimit = 2;
        constexpr std::uint32_t lowercaseOffset = 26;
        constexpr std::uint32_t digitOffset = 52;
        constexpr std::uint32_t plusValue = 62;
        constexpr std::uint32_t slashValue = 63;
        const auto valueOf = [](const char character) -> std::uint32_t {
            if (character >= 'A' && character <= 'Z') {
                return static_cast<std::uint32_t>(character - 'A');
            }

            if (character >= 'a' && character <= 'z') {
                return static_cast<std::uint32_t>(character - 'a') + lowercaseOffset;
            }

            if (character >= '0' && character <= '9') {
                return static_cast<std::uint32_t>(character - '0') + digitOffset;
            }

            if (character == '+') {
                return plusValue;
            }

            if (character == '/') {
                return slashValue;
            }

            throw std::invalid_argument{"The text is not valid Base64"};
        };
        const auto put = [&output](const std::uint32_t value) {
            *output = static_cast<char>(value & byteMask);
            ++output;
        };
        std::size_t length = text.size();
        std::size_t padding = 0;

        while (length > 0 && padding < paddingLimit && text.at(length - 1) == '=') {
            --length;
            ++padding;
        }

        if (padding > 0 && text.size() % charactersPerGroup != 0) {
            throw std::invalid_argument{"The Base64 padding is misplaced"};
        }

        if (length % charactersPerGroup == 1) {
            throw std::invalid_argument{"The Base64 text is truncated"};
        }

        std::uint32_t group = 0;
        std::size_t filled = 0;

        for (const char character : text.substr(0, length)) {
            group = (group << bitsPerCharacter) | valueOf(character);
            ++filled;

            if (filled == charactersPerGroup) {
                put(group >> (2 * bitsPerByte));
                put(group >> bitsPerByte);
                put(group);
                group = 0;
                filled = 0;
            }
        }

        if (filled == 2) {
            put(group >> ((2 * bitsPerCharacter) - bitsPerByte));
        } else if (filled == 3) {
            put(group >> ((3 * bitsPerCharacter) - bitsPerByte));
            put(group >> ((3 * bitsPerCharacter) - (2 * bitsPerByte)));
        }

        return output;
    }
} // namespace Consolpp::Text::Base64
