#include "Consolpp/Text/EmojiPresentationRanges.hpp"

#include "Consolpp/Text/ContainsCodePoint.hpp"

#include <cstdint>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::containsCodePoint;
    using Consolpp::Text::emojiPresentationRanges;

    TEST(EmojiPresentationRangesTest, ContainsEmojiShownAsEmoji)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x1F600;

        // Act
        const bool contained = containsCodePoint(emojiPresentationRanges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(EmojiPresentationRangesTest, ContainsRegionalIndicators)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x1F1E6;

        // Act
        const bool contained = containsCodePoint(emojiPresentationRanges, codePoint);

        // Assert
        EXPECT_TRUE(contained);
    }

    TEST(EmojiPresentationRangesTest, DoesNotContainEmojiShownAsText)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x2764;

        // Act
        const bool contained = containsCodePoint(emojiPresentationRanges, codePoint);

        // Assert
        EXPECT_FALSE(contained);
    }
} // namespace Consolpp::Tests::Unit::Text
