#pragma once

#include "Consolpp/Markup/DecodeError.hpp"
#include "Consolpp/Markup/DecodeErrorKind.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Consolpp::Markup
{
    // The reference is given as written, e.g. "&#x41;". The code point must be a Char of XML 1.0.
    constexpr std::uint32_t decodeCharacterReference(const std::string_view reference, const std::size_t position)
    {
        constexpr std::uint32_t decimalBase = 10;
        constexpr std::uint32_t hexBase = 16;
        constexpr std::uint32_t firstPrintable = 0x20;
        constexpr std::uint32_t surrogateStart = 0xD800;
        constexpr std::uint32_t surrogateEnd = 0xDFFF;
        constexpr std::uint32_t firstNonCharacter = 0xFFFE;
        constexpr std::uint32_t lastNonCharacter = 0xFFFF;
        constexpr std::uint32_t lastCodePoint = 0x10FFFF;
        const bool hexadecimal = reference.starts_with("&#x");
        const std::uint32_t base = hexadecimal ? hexBase : decimalBase;
        const std::size_t prefixLength = hexadecimal ? 3 : 2;
        std::uint32_t codePoint = 0;

        if (!reference.starts_with("&#") || !reference.ends_with(';') || reference.size() == prefixLength + 1) {
            throw DecodeError{DecodeErrorKind::InvalidCharacterReference, reference, position};
        }

        const std::string_view digits = reference.substr(prefixLength, reference.size() - prefixLength - 1);

        for (const char digit : digits) {
            std::uint32_t value = 0;

            if (digit >= '0' && digit <= '9') {
                value = static_cast<std::uint32_t>(digit - '0');
            } else if (hexadecimal && digit >= 'a' && digit <= 'f') {
                value = decimalBase + static_cast<std::uint32_t>(digit - 'a');
            } else if (hexadecimal && digit >= 'A' && digit <= 'F') {
                value = decimalBase + static_cast<std::uint32_t>(digit - 'A');
            } else {
                throw DecodeError{DecodeErrorKind::InvalidCharacterReference, reference, position};
            }

            codePoint = (codePoint * base) + value;

            if (codePoint > lastCodePoint) {
                throw DecodeError{DecodeErrorKind::InvalidCharacterReference, reference, position};
            }
        }

        const bool control = codePoint < firstPrintable && codePoint != '\t' && codePoint != '\n' && codePoint != '\r';
        const bool surrogate = codePoint >= surrogateStart && codePoint <= surrogateEnd;
        const bool nonCharacter = codePoint == firstNonCharacter || codePoint == lastNonCharacter;

        if (control || surrogate || nonCharacter) {
            throw DecodeError{DecodeErrorKind::InvalidCharacterReference, reference, position};
        }

        return codePoint;
    }
} // namespace Consolpp::Markup
