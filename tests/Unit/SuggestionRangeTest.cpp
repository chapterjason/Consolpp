#include "Consolpp/SuggestionRange.hpp"

#include <array>
#include <iterator>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    using Consolpp::SuggestionRange;

    namespace
    {
        constexpr std::array<std::string_view, 3> candidates{"cache:clear", "debug:router", "cache:warmup"};

        bool startsWithName(const std::string_view name, const std::string_view candidate)
        {
            return candidate.starts_with(name);
        }
    } // namespace

    TEST(SuggestionRangeTest, SatisfiesInputRange)
    {
        // Arrange
        using Range = SuggestionRange;

        // Act
        constexpr bool satisfied = std::ranges::input_range<Range>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(SuggestionRangeTest, BeginEqualsEndWithoutSuggestions)
    {
        // Arrange
        const SuggestionRange suggestions{"xyzzy", candidates, &startsWithName};

        // Act
        const bool empty = suggestions.begin() == SuggestionRange::end();

        // Assert
        EXPECT_TRUE(empty);
    }

    TEST(SuggestionRangeTest, RangeForVisitsEverySuggestionInOrder)
    {
        // Arrange
        const SuggestionRange suggestions{"cache", candidates, &startsWithName};
        const std::vector<std::string_view> expected{"cache:clear", "cache:warmup"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view suggestion : suggestions) {
            visited.push_back(suggestion);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(SuggestionRangeTest, CanBeIteratedAgain)
    {
        // Arrange
        const SuggestionRange suggestions{"cache", candidates, &startsWithName};
        const std::vector<std::string_view> expected{"cache:clear", "cache:warmup"};
        std::vector<std::string_view> visited;

        static_cast<void>(std::ranges::distance(suggestions));

        // Act
        for (const std::string_view suggestion : suggestions) {
            visited.push_back(suggestion);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }
} // namespace Consolpp::Tests::Unit
