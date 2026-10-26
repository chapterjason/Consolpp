#include "Consolpp/Styling/EncodeColorCode.hpp"

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Styling/ColorLayer.hpp"
#include "Consolpp/Terminal/ColorMode.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::Color;
    using Consolpp::Styling::ColorLayer;
    using Consolpp::Styling::encodeColorCode;
    using Consolpp::Terminal::ColorMode;

    TEST(EncodeColorCodeTest, WritesTheDefaultForeground)
    {
        // Arrange
        constexpr Color color = Color::defaultColor;
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Foreground, Terminal::ColorMode::Colors16, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "39");
    }

    TEST(EncodeColorCodeTest, WritesTheDefaultBackground)
    {
        // Arrange
        constexpr Color color = Color::defaultColor;
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Background, Terminal::ColorMode::Colors16, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "49");
    }

    TEST(EncodeColorCodeTest, WritesABasicForeground)
    {
        // Arrange
        constexpr Color color = Color::red;
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Foreground, Terminal::ColorMode::Colors16, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "31");
    }

    TEST(EncodeColorCodeTest, WritesABasicBackground)
    {
        // Arrange
        constexpr Color color = Color::cyan;
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Background, Terminal::ColorMode::Colors16, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "46");
    }

    TEST(EncodeColorCodeTest, WritesABrightForeground)
    {
        // Arrange
        constexpr Color color = Color::gray;
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Foreground, Terminal::ColorMode::Colors16, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "90");
    }

    TEST(EncodeColorCodeTest, WritesABrightBackground)
    {
        // Arrange
        constexpr Color color = Color::brightWhite;
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Background, Terminal::ColorMode::Colors16, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "107");
    }

    TEST(EncodeColorCodeTest, ReducesRgbToABasicColorFor16Colors)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Foreground, Terminal::ColorMode::Colors16, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "33");
    }

    TEST(EncodeColorCodeTest, ReducesRgbToABasicBackgroundFor16Colors)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Background, Terminal::ColorMode::Colors16, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "43");
    }

    TEST(EncodeColorCodeTest, ReducesRgbToThePaletteFor256Colors)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Foreground, Terminal::ColorMode::Colors256, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "38;5;214");
    }

    TEST(EncodeColorCodeTest, ReducesRgbToThePaletteBackgroundFor256Colors)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Background, Terminal::ColorMode::Colors256, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "48;5;214");
    }

    TEST(EncodeColorCodeTest, WritesTheChannelsForTrueColor)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Foreground, Terminal::ColorMode::TrueColor, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "38;2;255;128;1");
    }

    TEST(EncodeColorCodeTest, WritesTheBackgroundChannelsForTrueColor)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(0, 10, 200);
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Background, Terminal::ColorMode::TrueColor, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "48;2;0;10;200");
    }

    TEST(EncodeColorCodeTest, KeepsNamedColorsInEveryMode)
    {
        // Arrange
        constexpr Color color = Color::red;
        std::string written;

        // Act
        encodeColorCode(color, ColorLayer::Foreground, Terminal::ColorMode::TrueColor, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "31");
    }
} // namespace Consolpp::Tests::Unit::Styling
