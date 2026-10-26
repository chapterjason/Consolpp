#include "Consolpp/Text/Grapheme/Range.hpp"

#include <ranges>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Grapheme
{
    using Consolpp::Text::Grapheme::Range;

    TEST(RangeTest, SatisfiesInputRange)
    {
        // Arrange
        using Range = Range;

        // Act
        constexpr bool satisfied = std::ranges::input_range<Range>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(RangeTest, VisitsEveryCluster)
    {
        // Arrange
        const Range graphemes{"aé\U0001F600\U0001F1E9\U0001F1EA"};
        const std::vector<std::string_view> expected{"a", "é", "\U0001F600", "\U0001F1E9\U0001F1EA"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view cluster : graphemes) {
            visited.push_back(cluster);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(RangeTest, VisitsNothingForAnEmptyText)
    {
        // Arrange
        const Range graphemes{std::string_view{}};

        // Act
        const bool empty = graphemes.begin() == Range::end();

        // Assert
        EXPECT_TRUE(empty);
    }
} // namespace Consolpp::Tests::Unit::Text::Grapheme
