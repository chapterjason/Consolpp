#include "Consolpp/Markup/ParseColor.hpp"

#include "Consolpp/Markup/Color.hpp"

#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::parseColor;

    TEST(ParseColorTest, ParsesABasicColorName)
    {
        // Arrange
        constexpr std::string_view value = "cyan";

        // Act
        const Color color = parseColor(value);

        // Assert
        EXPECT_EQ(color, Color::cyan);
    }

    TEST(ParseColorTest, ParsesABrightColorName)
    {
        // Arrange
        constexpr std::string_view value = "bright-red";

        // Act
        const Color color = parseColor(value);

        // Assert
        EXPECT_EQ(color, Color::brightRed);
    }

    TEST(ParseColorTest, ParsesGray)
    {
        // Arrange
        constexpr std::string_view value = "gray";

        // Act
        const Color color = parseColor(value);

        // Assert
        EXPECT_EQ(color, Color::gray);
    }

    TEST(ParseColorTest, ParsesTheDefaultColor)
    {
        // Arrange
        constexpr std::string_view value = "default";

        // Act
        const Color color = parseColor(value);

        // Assert
        EXPECT_EQ(color, Color::defaultColor);
    }

    TEST(ParseColorTest, IgnoresTheCaseOfNames)
    {
        // Arrange
        constexpr std::string_view value = "Bright-RED";

        // Act
        const Color color = parseColor(value);

        // Assert
        EXPECT_EQ(color, Color::brightRed);
    }

    TEST(ParseColorTest, ParsesAHexColor)
    {
        // Arrange
        constexpr std::string_view value = "#FF8001";

        // Act
        const Color color = parseColor(value);

        // Assert
        EXPECT_EQ(color, Color::fromRgb(255, 128, 1));
    }

    TEST(ParseColorTest, ThrowsForAnUnknownName)
    {
        // Arrange
        constexpr std::string_view value = "purple";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(parseColor(value));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(ParseColorTest, ThrowsForAnEmptyValue)
    {
        // Arrange
        constexpr std::string_view value{};
        bool thrown = false;

        // Act
        try {
            static_cast<void>(parseColor(value));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(ParseColorTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr std::string_view value = "#f80";

        // Act
        constexpr Color color = parseColor(value);

        // Assert
        EXPECT_EQ(color, Color::fromRgb(255, 136, 0));
    }
} // namespace Consolpp::Tests::Unit::Markup
