#include "Consolpp/Text/LevenshteinDistance.hpp"

#include <cstddef>
#include <stdexcept>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::levenshteinDistance;

    TEST(LevenshteinDistanceTest, IsZeroForEqualTexts)
    {
        // Arrange
        constexpr std::size_t maxLength = 6;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("deploy", "deploy");

        // Assert
        EXPECT_EQ(distance, 0);
    }

    TEST(LevenshteinDistanceTest, IsTheLengthOfTheTargetForAnEmptySource)
    {
        // Arrange
        constexpr std::size_t maxLength = 6;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("", "deploy");

        // Assert
        EXPECT_EQ(distance, 6);
    }

    TEST(LevenshteinDistanceTest, IsTheLengthOfTheSourceForAnEmptyTarget)
    {
        // Arrange
        constexpr std::size_t maxLength = 0;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("deploy", "");

        // Assert
        EXPECT_EQ(distance, 6);
    }

    TEST(LevenshteinDistanceTest, CountsAnInsertion)
    {
        // Arrange
        constexpr std::size_t maxLength = 6;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("deplo", "deploy");

        // Assert
        EXPECT_EQ(distance, 1);
    }

    TEST(LevenshteinDistanceTest, CountsADeletion)
    {
        // Arrange
        constexpr std::size_t maxLength = 5;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("deploy", "deplo");

        // Assert
        EXPECT_EQ(distance, 1);
    }

    TEST(LevenshteinDistanceTest, CountsASubstitution)
    {
        // Arrange
        constexpr std::size_t maxLength = 6;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("deplay", "deploy");

        // Assert
        EXPECT_EQ(distance, 1);
    }

    TEST(LevenshteinDistanceTest, CombinesEdits)
    {
        // Arrange
        constexpr std::size_t maxLength = 7;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("kitten", "sitting");

        // Assert
        EXPECT_EQ(distance, 3);
    }

    TEST(LevenshteinDistanceTest, IsCaseSensitive)
    {
        // Arrange
        constexpr std::size_t maxLength = 6;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("Deploy", "deploy");

        // Assert
        EXPECT_EQ(distance, 1);
    }

    TEST(LevenshteinDistanceTest, AcceptsASourceLongerThanTheMaximumLength)
    {
        // Arrange
        constexpr std::size_t maxLength = 3;

        // Act
        const std::size_t distance = levenshteinDistance<maxLength>("deploy", "dep");

        // Assert
        EXPECT_EQ(distance, 3);
    }

    TEST(LevenshteinDistanceTest, ThrowsForATargetLongerThanTheMaximumLength)
    {
        // Arrange
        constexpr std::size_t maxLength = 5;
        bool thrown = false;

        // Act
        try {
            static_cast<void>(levenshteinDistance<maxLength>("deploy", "deploy"));
        } catch (const std::length_error&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(LevenshteinDistanceTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr std::size_t maxLength = 7;

        // Act
        constexpr std::size_t distance = levenshteinDistance<maxLength>("kitten", "sitting");

        // Assert
        EXPECT_EQ(distance, 3);
    }
} // namespace Consolpp::Tests::Unit::Text
