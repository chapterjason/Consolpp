#include "Consolpp/Markup/Color.hpp"

#include "Consolpp/Markup/ColorKind.hpp"

#include <cstddef>
#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::ColorKind;

    TEST(ColorTest, FromRgbKeepsTheChannels)
    {
        // Arrange
        constexpr Color expected{.kind = ColorKind::Rgb, .redChannel = 255, .greenChannel = 128, .blueChannel = 1};

        // Act
        constexpr Color color = Color::fromRgb(255, 128, 1);

        // Assert
        EXPECT_EQ(color, expected);
    }

    TEST(ColorTest, NamesTheBasicColorsByTheirAnsiIndex)
    {
        // Arrange
        constexpr Color expected{.kind = ColorKind::Basic, .index = 6};

        // Act
        constexpr Color color = Color::cyan;

        // Assert
        EXPECT_EQ(color, expected);
    }

    TEST(ColorTest, NamesTheBrightColorsByTheirAnsiIndex)
    {
        // Arrange
        constexpr Color expected{.kind = ColorKind::Bright, .index = 0};

        // Act
        constexpr Color color = Color::gray;

        // Assert
        EXPECT_EQ(color, expected);
    }

    TEST(ColorTest, NamesTheDefaultColor)
    {
        // Arrange
        constexpr Color expected{.kind = ColorKind::Default};

        // Act
        constexpr Color color = Color::defaultColor;

        // Assert
        EXPECT_EQ(color, expected);
    }

    TEST(ColorTest, FitsIntoFiveBytes)
    {
        // Arrange
        constexpr std::size_t expected = 5;

        // Act
        constexpr std::size_t size = sizeof(Color);

        // Assert
        EXPECT_EQ(size, expected);
    }

    TEST(ColorTest, FromHexReadsSixDigits)
    {
        // Arrange
        constexpr std::string_view value = "#ff8001";

        // Act
        const Color color = Color::fromHex(value);

        // Assert
        EXPECT_EQ(color, Color::fromRgb(255, 128, 1));
    }

    TEST(ColorTest, FromHexDoublesThreeDigits)
    {
        // Arrange
        constexpr std::string_view value = "#f80";

        // Act
        const Color color = Color::fromHex(value);

        // Assert
        EXPECT_EQ(color, Color::fromRgb(255, 136, 0));
    }

    TEST(ColorTest, FromHexIgnoresCase)
    {
        // Arrange
        constexpr std::string_view value = "#FfA0b1";

        // Act
        const Color color = Color::fromHex(value);

        // Assert
        EXPECT_EQ(color, Color::fromRgb(255, 160, 177));
    }

    TEST(ColorTest, FromHexThrowsWithoutHash)
    {
        // Arrange
        constexpr std::string_view value = "ff8001";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(Color::fromHex(value));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(ColorTest, FromHexThrowsForAWrongNumberOfDigits)
    {
        // Arrange
        constexpr std::string_view value = "#ff00";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(Color::fromHex(value));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(ColorTest, FromHexThrowsForAnInvalidDigit)
    {
        // Arrange
        constexpr std::string_view value = "#gg0000";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(Color::fromHex(value));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(ColorTest, FromHexIsUsableAtCompileTime)
    {
        // Arrange
        constexpr std::string_view value = "#f80";

        // Act
        constexpr Color color = Color::fromHex(value);

        // Assert
        EXPECT_EQ(color, Color::fromRgb(255, 136, 0));
    }
} // namespace Consolpp::Tests::Unit::Markup
