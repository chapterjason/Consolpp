#include "Consolpp/Text/CompareNaturalIgnoringCase.hpp"

#include <compare>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::compareNaturalIgnoringCase;

    TEST(CompareNaturalIgnoringCaseTest, OrdersNumbersByValue)
    {
        // Arrange
        constexpr std::string_view left = "file2";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "file10");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::less);
    }

    TEST(CompareNaturalIgnoringCaseTest, OrdersTheLargerNumberLast)
    {
        // Arrange
        constexpr std::string_view left = "x10";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "x9");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::greater);
    }

    TEST(CompareNaturalIgnoringCaseTest, OrdersNumbersOfTheSameLengthByValue)
    {
        // Arrange
        constexpr std::string_view left = "x21";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "x13");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::greater);
    }

    TEST(CompareNaturalIgnoringCaseTest, ContinuesAfterEqualNumbers)
    {
        // Arrange
        constexpr std::string_view left = "a1b";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "a1c");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::less);
    }

    TEST(CompareNaturalIgnoringCaseTest, IgnoresLeadingZeros)
    {
        // Arrange
        constexpr std::string_view left = "x007";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "x7");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::equivalent);
    }

    TEST(CompareNaturalIgnoringCaseTest, IgnoresCase)
    {
        // Arrange
        constexpr std::string_view left = "Deploy";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "deploy");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::equivalent);
    }

    TEST(CompareNaturalIgnoringCaseTest, OrdersLettersIgnoringCase)
    {
        // Arrange
        constexpr std::string_view left = "apple";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "Banana");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::less);
    }

    TEST(CompareNaturalIgnoringCaseTest, OrdersAPrefixFirst)
    {
        // Arrange
        constexpr std::string_view left = "cache";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "cache:clear");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::less);
    }

    TEST(CompareNaturalIgnoringCaseTest, OrdersByByteValueOtherwise)
    {
        // Arrange
        constexpr std::string_view left = "cache-all";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "cache:all");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::less);
    }

    TEST(CompareNaturalIgnoringCaseTest, TreatsEmptyTextsAsEquivalent)
    {
        // Arrange
        constexpr std::string_view left{};

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, std::string_view{});

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::equivalent);
    }

    TEST(CompareNaturalIgnoringCaseTest, OrdersAnEmptyTextFirst)
    {
        // Arrange
        constexpr std::string_view left{};

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "a");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::less);
    }

    TEST(CompareNaturalIgnoringCaseTest, ComparesDigitsWithLettersByByteValue)
    {
        // Arrange
        constexpr std::string_view left = "a1";

        // Act
        const std::weak_ordering ordering = compareNaturalIgnoringCase(left, "ab");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::less);
    }

    TEST(CompareNaturalIgnoringCaseTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr std::string_view left = "file2";

        // Act
        constexpr std::weak_ordering ordering = compareNaturalIgnoringCase(left, "file10");

        // Assert
        EXPECT_EQ(ordering, std::weak_ordering::less);
    }
} // namespace Consolpp::Tests::Unit::Text
