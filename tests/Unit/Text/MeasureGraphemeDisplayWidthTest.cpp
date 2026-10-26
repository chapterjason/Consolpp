#include "Consolpp/Text/MeasureGraphemeDisplayWidth.hpp"

#include <cstddef>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::measureGraphemeDisplayWidth;

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresAnAsciiLetterAsOne)
    {
        // Arrange
        constexpr std::string_view cluster = "a";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 1U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresAnAmbiguousLetterAsNarrow)
    {
        // Arrange
        constexpr std::string_view cluster = "\u00E9";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 1U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, IgnoresACombiningMark)
    {
        // Arrange
        constexpr std::string_view cluster = "e\u0301";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 1U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresCjkAsTwo)
    {
        // Arrange
        constexpr std::string_view cluster = "\u4E2D";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresHangulSyllablesAsTwo)
    {
        // Arrange
        constexpr std::string_view cluster = "\uAC00";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresALeadingJamoAsTwo)
    {
        // Arrange
        constexpr std::string_view cluster = "\u1100";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresAnEmojiAsTwo)
    {
        // Arrange
        constexpr std::string_view cluster = "\U0001F600";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresATextDefaultEmojiAsOne)
    {
        // Arrange
        constexpr std::string_view cluster = "\u2764";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 1U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, WidensATextDefaultEmojiWithVariationSelector16)
    {
        // Arrange
        constexpr std::string_view cluster = "\u2764\uFE0F";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, WidensAnEmojiWithASkinTone)
    {
        // Arrange
        constexpr std::string_view cluster = "\U0001F44D\U0001F3FD";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, WidensAKeycapWithVariationSelector16)
    {
        // Arrange
        constexpr std::string_view cluster = "1\uFE0F\u20E3";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, KeepsAKeycapWithoutVariationSelectorNarrow)
    {
        // Arrange
        constexpr std::string_view cluster = "1\u20E3";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 1U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresAZeroWidthJoinerSequenceAsTwo)
    {
        // Arrange
        constexpr std::string_view cluster = "\U0001F468\u200D\U0001F469\u200D\U0001F467";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresAFlagAsTwo)
    {
        // Arrange
        constexpr std::string_view cluster = "\U0001F1E9\U0001F1EA";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, KeepsAWideBaseWideWithTextPresentation)
    {
        // Arrange
        constexpr std::string_view cluster = "\u231A\uFE0E";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, NarrowsAnEmojiDefaultToTextWithVariationSelector15)
    {
        // Arrange
        constexpr std::string_view cluster = "\u2764\uFE0F\uFE0E";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 1U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresAZeroWidthSpaceAsZero)
    {
        // Arrange
        constexpr std::string_view cluster = "\u200B";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 0U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresASoftHyphenAsZero)
    {
        // Arrange
        constexpr std::string_view cluster = "\u00AD";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 0U);
    }

    TEST(MeasureGraphemeDisplayWidthTest, MeasuresAControlAsZero)
    {
        // Arrange
        constexpr std::string_view cluster = "\t";

        // Act
        const std::size_t width = measureGraphemeDisplayWidth(cluster);

        // Assert
        EXPECT_EQ(width, 0U);
    }
} // namespace Consolpp::Tests::Unit::Text
