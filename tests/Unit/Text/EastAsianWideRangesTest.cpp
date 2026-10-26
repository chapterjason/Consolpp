#include "Consolpp/Text/EastAsianWideRanges.hpp"

#include "Consolpp/Text/ContainsCodePoint.hpp"

#include <cstdint>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::containsCodePoint;
    using Consolpp::Text::eastAsianWideRanges;

    TEST(EastAsianWideRangesTest, ContainsCjkIdeographs)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x4E2D;

        // Act
        const bool contained = containsCodePoint(eastAsianWideRanges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(EastAsianWideRangesTest, ContainsHangulSyllables)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0xAC00;

        // Act
        const bool contained = containsCodePoint(eastAsianWideRanges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(EastAsianWideRangesTest, ContainsWideEmoji)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x1F600;

        // Act
        const bool contained = containsCodePoint(eastAsianWideRanges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(EastAsianWideRangesTest, DoesNotContainAmbiguousLetters)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0xE9;

        // Act
        const bool contained = containsCodePoint(eastAsianWideRanges, codePoint);

        // Assert
        EXPECT_FALSE(contained);
    }

    TEST(EastAsianWideRangesTest, DoesNotContainNarrowSymbols)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x2764;

        // Act
        const bool contained = containsCodePoint(eastAsianWideRanges, codePoint);

        // Assert
        EXPECT_FALSE(contained);
    }
} // namespace Consolpp::Tests::Unit::Text
