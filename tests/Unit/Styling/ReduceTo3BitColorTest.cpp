#include "Consolpp/Styling/ReduceTo3BitColor.hpp"

#include "Consolpp/Markup/Color.hpp"

#include <cstdint>
#include <stdexcept>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::Color;
    using Consolpp::Styling::reduceTo3BitColor;

    TEST(ReduceTo3BitColorTest, ReducesFF8001)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 3);
    }

    TEST(ReduceTo3BitColorTest, Reduces808080)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(128, 128, 128);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 7);
    }

    TEST(ReduceTo3BitColorTest, Reduces000000)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(0, 0, 0);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 0);
    }

    TEST(ReduceTo3BitColorTest, ReducesFFFFFF)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 255, 255);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 7);
    }

    TEST(ReduceTo3BitColorTest, Reduces050505)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(5, 5, 5);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 0);
    }

    TEST(ReduceTo3BitColorTest, ReducesFAFAFA)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(250, 250, 250);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 7);
    }

    TEST(ReduceTo3BitColorTest, Reduces123456)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(18, 52, 86);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 0);
    }

    TEST(ReduceTo3BitColorTest, Reduces00FF00)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(0, 255, 0);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 2);
    }

    TEST(ReduceTo3BitColorTest, Reduces7F7F80)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(127, 127, 128);

        // Act
        const std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 4);
    }

    TEST(ReduceTo3BitColorTest, ThrowsForANamedColor)
    {
        // Arrange
        constexpr Color color = Color::cyan;
        bool thrown = false;

        // Act
        try {
            static_cast<void>(reduceTo3BitColor(color));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(ReduceTo3BitColorTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr Color color = Color::fromRgb(255, 128, 1);

        // Act
        constexpr std::uint8_t index = reduceTo3BitColor(color);

        // Assert
        EXPECT_EQ(index, 3);
    }
} // namespace Consolpp::Tests::Unit::Styling
