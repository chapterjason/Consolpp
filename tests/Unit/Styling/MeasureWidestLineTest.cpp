#include "Consolpp/Styling/MeasureWidestLine.hpp"

#include "Consolpp/Markup/ParseError.hpp"

#include <cstddef>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::ParseError;
    using Consolpp::Styling::measureWidestLine;

    TEST(MeasureWidestLineTest, MeasuresTheVisibleText)
    {
        // Arrange
        std::string text;

        // Act
        const std::size_t width = measureWidestLine("<info>ab</info>&amp;", text);

        // Assert
        EXPECT_EQ(width, 3U);
        EXPECT_EQ(text, "ab&");
    }

    TEST(MeasureWidestLineTest, TakesTheWidestLine)
    {
        // Arrange
        std::string text;

        // Act
        const std::size_t width = measureWidestLine("ab\nabcd\nabc", text);

        // Assert
        EXPECT_EQ(width, 4U);
    }

    TEST(MeasureWidestLineTest, CountsWideCharactersTwice)
    {
        // Arrange
        std::string text;

        // Act
        const std::size_t width = measureWidestLine("\xE6\x97\xA5", text);

        // Assert
        EXPECT_EQ(width, 2U);
    }

    TEST(MeasureWidestLineTest, ThrowsForInvalidMarkup)
    {
        // Arrange
        std::string text;

        // Act
        const auto measure = [&text] {
            static_cast<void>(measureWidestLine("<info>a", text));
        };

        // Assert
        EXPECT_THROW(measure(), ParseError);
    }
} // namespace Consolpp::Tests::Unit::Styling
