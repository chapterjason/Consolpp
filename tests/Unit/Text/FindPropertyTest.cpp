#include "Consolpp/Text/FindProperty.hpp"

#include "Consolpp/Text/PropertyRange.hpp"

#include <array>
#include <cstdint>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::findProperty;
    using Consolpp::Text::PropertyRange;

    namespace
    {
        constexpr std::array<PropertyRange<char>, 2> ranges{
            {
                {.first = 0x10, .last = 0x1F, .value = 'a'},
                {.first = 0x30, .last = 0x30, .value = 'b'},
            },
        };
    } // namespace

    TEST(FindPropertyTest, FindsTheValueOfTheRange)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x1F;

        // Act
        const auto value = findProperty<char>(ranges, codePoint, '-');

        // Assert
        EXPECT_EQ(value, 'a');
    }

    TEST(FindPropertyTest, FindsARangeOfOneCodePoint)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x30;

        // Act
        const auto value = findProperty<char>(ranges, codePoint, '-');

        // Assert
        EXPECT_EQ(value, 'b');
    }

    TEST(FindPropertyTest, FallsBackBetweenRanges)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x20;

        // Act
        const auto value = findProperty<char>(ranges, codePoint, '-');

        // Assert
        EXPECT_EQ(value, '-');
    }

    TEST(FindPropertyTest, FallsBackAfterTheLastRange)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x31;

        // Act
        const auto value = findProperty<char>(ranges, codePoint, '-');

        // Assert
        EXPECT_EQ(value, '-');
    }
} // namespace Consolpp::Tests::Unit::Text
