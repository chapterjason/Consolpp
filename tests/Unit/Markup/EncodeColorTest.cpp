#include "Consolpp/Markup/EncodeColor.hpp"

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/ColorKind.hpp"
#include "Consolpp/Markup/ParseColor.hpp"

#include <iterator>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::ColorKind;
    using Consolpp::Markup::encodeColor;
    using Consolpp::Markup::parseColor;

    TEST(EncodeColorTest, EncodesTheDefaultColor)
    {
        // Arrange
        std::string encoded;

        // Act
        encodeColor(Color::defaultColor, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "default");
    }

    TEST(EncodeColorTest, EncodesABasicColorByName)
    {
        // Arrange
        std::string encoded;

        // Act
        encodeColor(Color::magenta, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "magenta");
    }

    TEST(EncodeColorTest, EncodesABrightColorByName)
    {
        // Arrange
        std::string encoded;

        // Act
        encodeColor(Color::brightCyan, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "bright-cyan");
    }

    TEST(EncodeColorTest, EncodesGrayAsTheFirstBrightColor)
    {
        // Arrange
        std::string encoded;

        // Act
        encodeColor(Color::gray, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "gray");
    }

    TEST(EncodeColorTest, EncodesAnRgbColorAsLowercaseHex)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(0x0A, 0xBC, 0xFF);
        std::string encoded;

        // Act
        encodeColor(color, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "#0abcff");
    }

    TEST(EncodeColorTest, IsReadBackAsTheSameColor)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(0x12, 0x34, 0x56);
        std::string encoded;

        // Act
        encodeColor(color, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(parseColor(encoded), color);
    }

    TEST(EncodeColorTest, ThrowsForAnIndexAboveSeven)
    {
        // Arrange
        constexpr Color color{.kind = ColorKind::Basic, .index = 8};
        std::string encoded;

        // Act
        const auto encode = [&color, &encoded] {
            encodeColor(color, std::back_inserter(encoded));
        };

        // Assert
        EXPECT_THROW(encode(), std::out_of_range);
    }
} // namespace Consolpp::Tests::Unit::Markup
