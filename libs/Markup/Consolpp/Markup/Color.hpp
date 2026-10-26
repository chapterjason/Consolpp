#pragma once

#include "Consolpp/Markup/ColorKind.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Markup
{
    struct Color
    {
        public:
            ColorKind kind;
            std::uint8_t index{0};
            std::uint8_t redChannel{0};
            std::uint8_t greenChannel{0};
            std::uint8_t blueChannel{0};

            static const Color defaultColor;
            static const Color black;
            static const Color red;
            static const Color green;
            static const Color yellow;
            static const Color blue;
            static const Color magenta;
            static const Color cyan;
            static const Color white;
            static const Color gray;
            static const Color brightRed;
            static const Color brightGreen;
            static const Color brightYellow;
            static const Color brightBlue;
            static const Color brightMagenta;
            static const Color brightCyan;
            static const Color brightWhite;

            static constexpr Color fromRgb(const std::uint8_t redChannel,
                                           const std::uint8_t greenChannel,
                                           const std::uint8_t blueChannel)
            {
                return Color{
                    .kind = ColorKind::Rgb,
                    .redChannel = redChannel,
                    .greenChannel = greenChannel,
                    .blueChannel = blueChannel,
                };
            }

            static constexpr Color fromHex(const std::string_view value)
            {
                constexpr std::uint8_t hexBase = 16;
                constexpr std::uint8_t firstLetterValue = 10;
                constexpr std::size_t shortHexLength = 3;
                constexpr std::size_t longHexLength = 6;
                const auto hexValue = [](const char character) -> std::uint8_t {
                    if (character >= '0' && character <= '9') {
                        return static_cast<std::uint8_t>(character - '0');
                    }

                    if (character >= 'a' && character <= 'f') {
                        return static_cast<std::uint8_t>(character - 'a' + firstLetterValue);
                    }

                    if (character >= 'A' && character <= 'F') {
                        return static_cast<std::uint8_t>(character - 'A' + firstLetterValue);
                    }

                    throw std::invalid_argument{"A hex color must only contain hex digits"};
                };
                const auto channel = [&hexValue](const char high, const char low) {
                    return static_cast<std::uint8_t>((hexValue(high) * hexBase) + hexValue(low));
                };

                if (!value.starts_with('#')) {
                    throw std::invalid_argument{"A hex color must start with #"};
                }

                const std::string_view digits = value.substr(1);

                if (digits.size() == shortHexLength) {
                    const auto doubledDigit = [&channel, &digits](const std::size_t index) {
                        return channel(digits.at(index), digits.at(index));
                    };

                    return Color::fromRgb(doubledDigit(0), doubledDigit(1), doubledDigit(2));
                }

                if (digits.size() == longHexLength) {
                    const auto digitPair = [&channel, &digits](const std::size_t index) {
                        return channel(digits.at(2 * index), digits.at((2 * index) + 1));
                    };

                    return Color::fromRgb(digitPair(0), digitPair(1), digitPair(2));
                }

                throw std::invalid_argument{"A hex color must have 3 or 6 digits"};
            }

            constexpr bool operator==(const Color& other) const = default;
    };

    inline constexpr Color Color::defaultColor{.kind = ColorKind::Default};
    inline constexpr Color Color::black{.kind = ColorKind::Basic, .index = 0};
    inline constexpr Color Color::red{.kind = ColorKind::Basic, .index = 1};
    inline constexpr Color Color::green{.kind = ColorKind::Basic, .index = 2};
    inline constexpr Color Color::yellow{.kind = ColorKind::Basic, .index = 3};
    inline constexpr Color Color::blue{.kind = ColorKind::Basic, .index = 4};
    inline constexpr Color Color::magenta{.kind = ColorKind::Basic, .index = 5};
    inline constexpr Color Color::cyan{.kind = ColorKind::Basic, .index = 6};
    inline constexpr Color Color::white{.kind = ColorKind::Basic, .index = 7};
    inline constexpr Color Color::gray{.kind = ColorKind::Bright, .index = 0};
    inline constexpr Color Color::brightRed{.kind = ColorKind::Bright, .index = 1};
    inline constexpr Color Color::brightGreen{.kind = ColorKind::Bright, .index = 2};
    inline constexpr Color Color::brightYellow{.kind = ColorKind::Bright, .index = 3};
    inline constexpr Color Color::brightBlue{.kind = ColorKind::Bright, .index = 4};
    inline constexpr Color Color::brightMagenta{.kind = ColorKind::Bright, .index = 5};
    inline constexpr Color Color::brightCyan{.kind = ColorKind::Bright, .index = 6};
    inline constexpr Color Color::brightWhite{.kind = ColorKind::Bright, .index = 7};
} // namespace Consolpp::Markup
