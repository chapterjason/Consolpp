#include "Consolpp/Screen/CountRows.hpp"

#include <cstddef>
#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Screen
{
    using Consolpp::Screen::countRows;

    TEST(CountRowsTest, TakesOneRowForAnEmptyLine)
    {
        // Arrange
        constexpr std::string_view line{};
        constexpr std::size_t width = 10;

        // Act
        const std::size_t rows = countRows(line, width);

        // Assert
        EXPECT_EQ(rows, 1U);
    }

    TEST(CountRowsTest, TakesOneRowForALineThatFits)
    {
        // Arrange
        constexpr std::string_view line = "abc";
        constexpr std::size_t width = 3;

        // Act
        const std::size_t rows = countRows(line, width);

        // Assert
        EXPECT_EQ(rows, 1U);
    }

    TEST(CountRowsTest, WrapsTheFirstCharacterThatDoesNotFit)
    {
        // Arrange
        constexpr std::string_view line = "abcd";
        constexpr std::size_t width = 3;

        // Act
        const std::size_t rows = countRows(line, width);

        // Assert
        EXPECT_EQ(rows, 2U);
    }

    TEST(CountRowsTest, TakesExactlyTheFilledRows)
    {
        // Arrange
        constexpr std::string_view line = "abcdef";
        constexpr std::size_t width = 3;

        // Act
        const std::size_t rows = countRows(line, width);

        // Assert
        EXPECT_EQ(rows, 2U);
    }

    TEST(CountRowsTest, StartsAThirdRow)
    {
        // Arrange
        constexpr std::string_view line = "abcdefg";
        constexpr std::size_t width = 3;

        // Act
        const std::size_t rows = countRows(line, width);

        // Assert
        EXPECT_EQ(rows, 3U);
    }

    TEST(CountRowsTest, MovesAWideCharacterToTheNextRow)
    {
        // Arrange
        constexpr std::string_view line = "ab\u4E2D";
        constexpr std::size_t width = 3;

        // Act
        const std::size_t rows = countRows(line, width);

        // Assert
        EXPECT_EQ(rows, 2U);
    }

    TEST(CountRowsTest, CountsATabAsMovingToTheNextStop)
    {
        // Arrange
        constexpr std::string_view line = "\tx";
        constexpr std::size_t width = 10;

        // Act
        const std::size_t rows = countRows(line, width);

        // Assert
        EXPECT_EQ(rows, 1U);
    }

    TEST(CountRowsTest, NeverWrapsAtATab)
    {
        // Arrange
        constexpr std::string_view line = "abcdefg\tx";
        constexpr std::size_t width = 8;

        // Act
        const std::size_t rows = countRows(line, width);

        // Assert
        EXPECT_EQ(rows, 2U);
    }

    TEST(CountRowsTest, ThrowsForAWidthOfZero)
    {
        // Arrange
        constexpr std::string_view line = "a";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(countRows(line, 0));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }
} // namespace Consolpp::Tests::Unit::Screen
