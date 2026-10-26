#include "Consolpp/SuggestionIterator.hpp"

#include <array>
#include <iterator>
#include <span>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    using Consolpp::SuggestionIterator;

    namespace
    {
        constexpr std::array<std::string_view, 4> candidates{"cache:clear", "debug:router", "cache:warmup", "list"};

        bool startsWithName(const std::string_view name, const std::string_view candidate)
        {
            return candidate.starts_with(name);
        }
    } // namespace

    TEST(SuggestionIteratorTest, SatisfiesInputIterator)
    {
        // Arrange
        using Iterator = SuggestionIterator;

        // Act
        constexpr bool satisfied = std::input_iterator<Iterator>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(SuggestionIteratorTest, EqualsTheSentinelWithoutCandidates)
    {
        // Arrange
        constexpr std::span<const std::string_view> none{};

        // Act
        const SuggestionIterator iterator{"cache", none, &startsWithName};

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }

    TEST(SuggestionIteratorTest, EqualsTheSentinelWhenNoCandidateIsASuggestion)
    {
        // Arrange
        constexpr std::string_view name = "xyzzy";

        // Act
        const SuggestionIterator iterator{name, candidates, &startsWithName};

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }

    TEST(SuggestionIteratorTest, VisitsTheSuggestionsInCandidateOrder)
    {
        // Arrange
        const std::vector<std::string_view> expected{"cache:clear", "cache:warmup"};
        std::vector<std::string_view> visited;

        // Act
        for (SuggestionIterator iterator{"cache", candidates, &startsWithName}; iterator != std::default_sentinel;
             ++iterator) {
            visited.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(SuggestionIteratorTest, VisitsASuggestionAfterTheLastRejectedCandidate)
    {
        // Arrange
        const std::vector<std::string_view> expected{"list"};
        std::vector<std::string_view> visited;

        // Act
        for (SuggestionIterator iterator{"li", candidates, &startsWithName}; iterator != std::default_sentinel;
             ++iterator) {
            visited.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }
} // namespace Consolpp::Tests::Unit
