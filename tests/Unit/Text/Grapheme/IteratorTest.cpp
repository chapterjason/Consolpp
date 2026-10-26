#include "Consolpp/Text/Grapheme/Iterator.hpp"

#include <iterator>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Grapheme
{
    using Consolpp::Text::Grapheme::Iterator;

    TEST(IteratorTest, SatisfiesInputIterator)
    {
        // Arrange
        using Iterator = Iterator;

        // Act
        constexpr bool satisfied = std::input_iterator<Iterator>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(IteratorTest, EqualsTheSentinelForAnEmptyText)
    {
        // Arrange
        constexpr std::string_view text{};

        // Act
        const Iterator iterator{text};

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }

    TEST(IteratorTest, YieldsTheFirstCluster)
    {
        // Arrange
        const Iterator iterator{"éx"};

        // Act
        const std::string_view cluster = *iterator;

        // Assert
        EXPECT_EQ(cluster, "é");
    }

    TEST(IteratorTest, AdvancesToTheNextCluster)
    {
        // Arrange
        Iterator iterator{"éx"};

        // Act
        ++iterator;

        // Assert
        EXPECT_EQ(*iterator, "x");
    }
} // namespace Consolpp::Tests::Unit::Text::Grapheme
