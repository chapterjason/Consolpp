#include "Consolpp/Text/ContainsCodePoint.hpp"

#include "Consolpp/Text/CodePointRange.hpp"

#include <array>
#include <cstdint>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::CodePointRange;
    using Consolpp::Text::containsCodePoint;

    namespace
    {
        constexpr std::array<CodePointRange, 3> ranges{
            {{.first = 0x10, .last = 0x1F}, {.first = 0x30, .last = 0x30}, {.first = 0x40, .last = 0x4F}},
        };
    } // namespace

    TEST(ContainsCodePointTest, FindsTheFirstCodePointOfARange)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x10;

        // Act
        const bool contained = containsCodePoint(ranges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(ContainsCodePointTest, FindsTheLastCodePointOfARange)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x4F;

        // Act
        const bool contained = containsCodePoint(ranges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(ContainsCodePointTest, FindsARangeOfOneCodePoint)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x30;

        // Act
        const bool contained = containsCodePoint(ranges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(ContainsCodePointTest, MissesACodePointBetweenRanges)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x20;

        // Act
        const bool contained = containsCodePoint(ranges, codePoint);

        // Assert
        EXPECT_FALSE(contained);
    }

    TEST(ContainsCodePointTest, MissesACodePointBeforeTheFirstRange)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x0F;

        // Act
        const bool contained = containsCodePoint(ranges, codePoint);

        // Assert
        EXPECT_FALSE(contained);
    }

    TEST(ContainsCodePointTest, MissesACodePointAfterTheLastRange)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x50;

        // Act
        const bool contained = containsCodePoint(ranges, codePoint);

        // Assert
        EXPECT_FALSE(contained);
    }
} // namespace Consolpp::Tests::Unit::Text
