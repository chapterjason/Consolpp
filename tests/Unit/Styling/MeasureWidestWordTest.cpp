#include "Consolpp/Styling/MeasureWidestWord.hpp"

#include <cstddef>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Styling::measureWidestWord;

    TEST(MeasureWidestWordTest, TakesTheWidestWordOfAllLines)
    {
        // Arrange
        std::string text;

        // Act
        const std::size_t width = measureWidestWord("ab <info>abcd</info>\nabc", text);

        // Assert
        EXPECT_EQ(width, 4U);
    }

    TEST(MeasureWidestWordTest, CountsWideCharactersTwice)
    {
        // Arrange
        std::string text;

        // Act
        const std::size_t width = measureWidestWord("a \xE6\x97\xA5\xE6\x9C\xAC", text);

        // Assert
        EXPECT_EQ(width, 4U);
    }

    TEST(MeasureWidestWordTest, IsZeroWithoutText)
    {
        // Arrange
        std::string text;

        // Act
        const std::size_t width = measureWidestWord("", text);

        // Assert
        EXPECT_EQ(width, 0U);
    }
} // namespace Consolpp::Tests::Unit::Styling
