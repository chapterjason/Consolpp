#pragma once

#include <cstdint>
#include <iterator>
#include <stdexcept>

namespace Consolpp::Text::Utf8
{
    template <std::output_iterator<char> Output>
    constexpr Output encode(const std::uint32_t codePoint, Output output)
    {
        constexpr std::uint32_t oneByteLimit = 0x80;
        constexpr std::uint32_t twoByteLimit = 0x800;
        constexpr std::uint32_t threeByteLimit = 0x10000;
        constexpr std::uint32_t codePointLimit = 0x110000;
        constexpr std::uint32_t surrogateStart = 0xD800;
        constexpr std::uint32_t surrogateEnd = 0xDFFF;
        constexpr std::uint32_t continuationMarker = 0x80;
        constexpr std::uint32_t continuationMask = 0x3F;
        constexpr std::uint32_t twoByteMarker = 0xC0;
        constexpr std::uint32_t threeByteMarker = 0xE0;
        constexpr std::uint32_t fourByteMarker = 0xF0;
        constexpr std::uint32_t bitsPerContinuation = 6;
        const auto put = [&output](const std::uint32_t byte) {
            *output = static_cast<char>(static_cast<unsigned char>(byte));
            ++output;
        };
        const auto putContinuation = [&put](const std::uint32_t bits) {
            put(continuationMarker | (bits & continuationMask));
        };

        if (codePoint >= codePointLimit || (codePoint >= surrogateStart && codePoint <= surrogateEnd)) {
            throw std::invalid_argument{"The code point cannot be encoded as UTF-8"};
        }

        if (codePoint < oneByteLimit) {
            put(codePoint);
        } else if (codePoint < twoByteLimit) {
            put(twoByteMarker | (codePoint >> bitsPerContinuation));
            putContinuation(codePoint);
        } else if (codePoint < threeByteLimit) {
            put(threeByteMarker | (codePoint >> (2 * bitsPerContinuation)));
            putContinuation(codePoint >> bitsPerContinuation);
            putContinuation(codePoint);
        } else {
            put(fourByteMarker | (codePoint >> (3 * bitsPerContinuation)));
            putContinuation(codePoint >> (2 * bitsPerContinuation));
            putContinuation(codePoint >> bitsPerContinuation);
            putContinuation(codePoint);
        }

        return output;
    }
} // namespace Consolpp::Text::Utf8
