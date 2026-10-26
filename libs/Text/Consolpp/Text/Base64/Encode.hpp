#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string_view>

namespace Consolpp::Text::Base64
{
    // RFC 4648 Base64 with padding.
    template <std::output_iterator<char> Output>
    constexpr Output encode(const std::string_view bytes, Output output)
    {
        constexpr std::string_view alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
        constexpr std::uint32_t sixBits = 0x3F;
        constexpr std::uint32_t bitsPerCharacter = 6;
        constexpr std::uint32_t bitsPerByte = 8;
        constexpr std::size_t bytesPerGroup = 3;
        constexpr std::size_t charactersPerGroup = 4;
        const auto byteAt = [bytes](const std::size_t index) {
            return static_cast<std::uint32_t>(static_cast<unsigned char>(bytes.at(index)));
        };

        for (std::size_t index = 0; index < bytes.size(); index += bytesPerGroup) {
            const std::size_t count = std::min(bytesPerGroup, bytes.size() - index);
            std::uint32_t group = 0;

            for (std::size_t offset = 0; offset < bytesPerGroup; ++offset) {
                group = (group << bitsPerByte) | (offset < count ? byteAt(index + offset) : 0);
            }

            for (std::size_t position = 0; position < charactersPerGroup; ++position) {
                const auto shift = static_cast<std::uint32_t>(bitsPerCharacter * (bytesPerGroup - position));

                *output = position <= count ? alphabet.at((group >> shift) & sixBits) : '=';
                ++output;
            }
        }

        return output;
    }
} // namespace Consolpp::Text::Base64
