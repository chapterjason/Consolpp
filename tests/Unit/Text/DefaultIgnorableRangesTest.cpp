#include "Consolpp/Text/DefaultIgnorableRanges.hpp"

#include "Consolpp/Text/ContainsCodePoint.hpp"

#include <cstdint>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::containsCodePoint;
    using Consolpp::Text::defaultIgnorableRanges;

    TEST(DefaultIgnorableRangesTest, ContainsTheZeroWidthSpace)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x200B;

        // Act
        const bool contained = containsCodePoint(defaultIgnorableRanges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(DefaultIgnorableRangesTest, ContainsVariationSelectors)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0xFE0F;

        // Act
        const bool contained = containsCodePoint(defaultIgnorableRanges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(DefaultIgnorableRangesTest, DoesNotContainLetters)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x41;

        // Act
        const bool contained = containsCodePoint(defaultIgnorableRanges, codePoint);

        // Assert
        EXPECT_FALSE(contained);
    }
} // namespace Consolpp::Tests::Unit::Text
