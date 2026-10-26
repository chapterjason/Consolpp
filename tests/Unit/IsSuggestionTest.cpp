#include "Consolpp/IsSuggestion.hpp"

#include <cstddef>
#include <stdexcept>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    using Consolpp::isSuggestion;

    namespace
    {
        constexpr std::size_t maxLength = 16;
    } // namespace

    TEST(IsSuggestionTest, SuggestsACandidateWithATypoInTheLastPart)
    {
        // Arrange
        constexpr const char* candidate = "cache:clear";

        // Act
        const bool suggested = isSuggestion<maxLength>("cache:cler", candidate);

        // Assert
        EXPECT_TRUE(suggested);
    }

    TEST(IsSuggestionTest, SuggestsACandidateInTheSameNamespace)
    {
        // Arrange
        constexpr const char* candidate = "cache:warmup";

        // Act
        const bool suggested = isSuggestion<maxLength>("cache:cler", candidate);

        // Assert
        EXPECT_TRUE(suggested);
    }

    TEST(IsSuggestionTest, RejectsACandidateInAnotherNamespace)
    {
        // Arrange
        constexpr const char* candidate = "debug:router";

        // Act
        const bool suggested = isSuggestion<maxLength>("cache:cler", candidate);

        // Assert
        EXPECT_FALSE(suggested);
    }

    TEST(IsSuggestionTest, RejectsAnUnrelatedCandidate)
    {
        // Arrange
        constexpr const char* candidate = "list";

        // Act
        const bool suggested = isSuggestion<maxLength>("xyzzy", candidate);

        // Assert
        EXPECT_FALSE(suggested);
    }

    TEST(IsSuggestionTest, SuggestsACandidateWithinAThirdOfTheLength)
    {
        // Arrange
        constexpr const char* candidate = "list";

        // Act
        const bool suggested = isSuggestion<maxLength>("lst", candidate);

        // Assert
        EXPECT_TRUE(suggested);
    }

    TEST(IsSuggestionTest, SuggestsACandidateWithMorePartsStartingWithTheName)
    {
        // Arrange
        constexpr const char* candidate = "deploy:all";

        // Act
        const bool suggested = isSuggestion<maxLength>("deploy", candidate);

        // Assert
        EXPECT_TRUE(suggested);
    }

    TEST(IsSuggestionTest, SuggestsACandidateContainingTheName)
    {
        // Arrange
        constexpr const char* candidate = "cache:clear";

        // Act
        const bool suggested = isSuggestion<maxLength>("cache", candidate);

        // Assert
        EXPECT_TRUE(suggested);
    }

    TEST(IsSuggestionTest, SuggestsACandidateWithOnePartLess)
    {
        // Arrange
        constexpr const char* candidate = "cache:clear";

        // Act
        const bool suggested = isSuggestion<maxLength>("cache:clear:x", candidate);

        // Assert
        EXPECT_TRUE(suggested);
    }

    TEST(IsSuggestionTest, RejectsACandidateWhereNoPartMatches)
    {
        // Arrange
        constexpr const char* candidate = "x";

        // Act
        const bool suggested = isSuggestion<maxLength>("a:b:c", candidate);

        // Assert
        EXPECT_FALSE(suggested);
    }

    TEST(IsSuggestionTest, SuggestsNothingForAnEmptyName)
    {
        // Arrange
        constexpr const char* candidate = "list";

        // Act
        const bool suggested = isSuggestion<maxLength>("", candidate);

        // Assert
        EXPECT_FALSE(suggested);
    }

    TEST(IsSuggestionTest, ComparesCaseSensitively)
    {
        // Arrange
        constexpr const char* candidate = "list";

        // Act
        const bool suggested = isSuggestion<maxLength>("LIST", candidate);

        // Assert
        EXPECT_FALSE(suggested);
    }

    TEST(IsSuggestionTest, RejectsACandidateWithTwoDifferentParts)
    {
        // Arrange
        constexpr const char* candidate = "a:x:y";

        // Act
        const bool suggested = isSuggestion<maxLength>("a:b:c", candidate);

        // Assert
        EXPECT_FALSE(suggested);
    }

    TEST(IsSuggestionTest, SuggestsACandidateWithASwappedPart)
    {
        // Arrange
        constexpr const char* candidate = "debug:router";

        // Act
        const bool suggested = isSuggestion<maxLength>("debug:rotuer", candidate);

        // Assert
        EXPECT_TRUE(suggested);
    }

    TEST(IsSuggestionTest, ThrowsForACandidateLongerThanTheMaximumLength)
    {
        // Arrange
        constexpr const char* candidate = "cache:pool:clear:all";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(isSuggestion<maxLength>("cache", candidate));
        } catch (const std::length_error&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(IsSuggestionTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr const char* candidate = "cache:clear";

        // Act
        constexpr bool suggested = isSuggestion<maxLength>("cache:cler", candidate);

        // Assert
        EXPECT_TRUE(suggested);
    }
} // namespace Consolpp::Tests::Unit
