#include "Consolpp/Text/MeasureDisplayWidth.hpp"

#include <cstddef>
#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::measureDisplayWidth;

    TEST(MeasureDisplayWidthTest, IsZeroForAnEmptyText)
    {
        // Arrange
        constexpr std::string_view text{};

        // Act
        const std::size_t width = measureDisplayWidth(text);

        // Assert
        EXPECT_EQ(width, 0U);
    }

    TEST(MeasureDisplayWidthTest, CountsPrintableAscii)
    {
        // Arrange
        constexpr std::string_view text = "hello";

        // Act
        const std::size_t width = measureDisplayWidth(text);

        // Assert
        EXPECT_EQ(width, 5U);
    }

    TEST(MeasureDisplayWidthTest, SumsTheClusters)
    {
        // Arrange
        constexpr std::string_view text = "caf\u00E9 \u4E2D\u6587";

        // Act
        const std::size_t width = measureDisplayWidth(text);

        // Assert
        EXPECT_EQ(width, 9U);
    }

    TEST(MeasureDisplayWidthTest, CountsAnEmojiSequenceOnce)
    {
        // Arrange
        constexpr std::string_view text = "\U0001F468\u200D\U0001F469\u200D\U0001F467 ok";

        // Act
        const std::size_t width = measureDisplayWidth(text);

        // Assert
        EXPECT_EQ(width, 5U);
    }

    TEST(MeasureDisplayWidthTest, IgnoresControls)
    {
        // Arrange
        constexpr std::string_view text = "a\tb";

        // Act
        const std::size_t width = measureDisplayWidth(text);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureDisplayWidthTest, ThrowsForInvalidUtf8)
    {
        // Arrange
        constexpr std::string_view text = "ok\xFF";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(measureDisplayWidth(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(MeasureDisplayWidthTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr std::string_view text = "\u4E2D\u6587";

        // Act
        constexpr std::size_t width = measureDisplayWidth(text);

        // Assert
        EXPECT_EQ(width, 4U);
    }
} // namespace Consolpp::Tests::Unit::Text
