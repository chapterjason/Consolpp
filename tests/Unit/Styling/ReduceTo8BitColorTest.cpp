#include "Consolpp/Styling/ReduceTo8BitColor.hpp"

#include "Consolpp/Markup/Color.hpp"

#include <cstdint>
#include <stdexcept>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::Color;
    using Consolpp::Styling::reduceTo8BitColor;

    TEST(ReduceTo8BitColorTest, ReducesFF8001)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 214);
    }

    TEST(ReduceTo8BitColorTest, Reduces808080)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(128, 128, 128);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 244);
    }

    TEST(ReduceTo8BitColorTest, Reduces000000)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(0, 0, 0);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 16);
    }

    TEST(ReduceTo8BitColorTest, ReducesFFFFFF)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 255, 255);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 231);
    }

    TEST(ReduceTo8BitColorTest, Reduces050505)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(5, 5, 5);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 16);
    }

    TEST(ReduceTo8BitColorTest, ReducesFAFAFA)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(250, 250, 250);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 231);
    }

    TEST(ReduceTo8BitColorTest, Reduces123456)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(18, 52, 86);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 24);
    }

    TEST(ReduceTo8BitColorTest, Reduces00FF00)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(0, 255, 0);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 46);
    }

    TEST(ReduceTo8BitColorTest, Reduces7F7F80)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(127, 127, 128);

        // Act
        const std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 103);
    }

    TEST(ReduceTo8BitColorTest, ThrowsForANamedColor)
    {
        // Arrange
        constexpr Color color = Color::cyan;
        bool thrown = false;

        // Act
        try {
            static_cast<void>(reduceTo8BitColor(color));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(ReduceTo8BitColorTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);

        // Act
        constexpr std::uint8_t index = reduceTo8BitColor(color);

        // Assert
        EXPECT_EQ(index, 214);
    }
} // namespace Consolpp::Tests::Unit::Styling
