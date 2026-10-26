#pragma once

#include "Consolpp/Text/Utf8/CodePoint.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Text::Utf8
{
    constexpr CodePoint decode(const std::string_view text)
    {
        constexpr std::uint32_t continuationMask = 0x3F;
        constexpr std::uint32_t continuationMarker = 0x80;
        constexpr std::uint32_t continuationCheckMask = 0xC0;
        constexpr std::uint32_t bitsPerContinuation = 6;
        constexpr std::uint32_t twoByteMask = 0xE0;
        constexpr std::uint32_t twoByteMarker = 0xC0;
        constexpr std::uint32_t threeByteMask = 0xF0;
        constexpr std::uint32_t threeByteMarker = 0xE0;
        constexpr std::uint32_t fourByteMask = 0xF8;
        constexpr std::uint32_t fourByteMarker = 0xF0;
        constexpr std::uint32_t oneByteLimit = 0x80;
        constexpr std::uint32_t twoByteMinimum = 0x80;
        constexpr std::uint32_t threeByteMinimum = 0x800;
        constexpr std::uint32_t fourByteMinimum = 0x10000;
        constexpr std::uint32_t codePointLimit = 0x110000;
        constexpr std::uint32_t surrogateStart = 0xD800;
        constexpr std::uint32_t surrogateEnd = 0xDFFF;
        const auto byteAt = [text](const std::size_t index) {
            return static_cast<std::uint32_t>(static_cast<unsigned char>(text.at(index)));
        };

        if (text.empty()) {
            throw std::invalid_argument{"There is no code point to decode"};
        }

        const std::uint32_t first = byteAt(0);

        if (first < oneByteLimit) {
            return CodePoint{.value = first, .length = 1};
        }

        std::size_t length = 0;
        std::uint32_t value = 0;
        std::uint32_t minimum = 0;

        if ((first & twoByteMask) == twoByteMarker) {
            length = 2;
            value = first & ~twoByteMask;
            minimum = twoByteMinimum;
        } else if ((first & threeByteMask) == threeByteMarker) {
            length = 3;
            value = first & ~threeByteMask;
            minimum = threeByteMinimum;
        } else if ((first & fourByteMask) == fourByteMarker) {
            length = 4;
            value = first & ~fourByteMask;
            minimum = fourByteMinimum;
        } else {
            throw std::invalid_argument{"The text is not valid UTF-8"};
        }

        if (text.size() < length) {
            throw std::invalid_argument{"The text is not valid UTF-8"};
        }

        for (std::size_t index = 1; index < length; ++index) {
            const std::uint32_t byte = byteAt(index);

            if ((byte & continuationCheckMask) != continuationMarker) {
                throw std::invalid_argument{"The text is not valid UTF-8"};
            }

            value = (value << bitsPerContinuation) | (byte & continuationMask);
        }

        if (value < minimum || value >= codePointLimit || (value >= surrogateStart && value <= surrogateEnd)) {
            throw std::invalid_argument{"The text is not valid UTF-8"};
        }

        return CodePoint{.value = value, .length = length};
    }
} // namespace Consolpp::Text::Utf8
