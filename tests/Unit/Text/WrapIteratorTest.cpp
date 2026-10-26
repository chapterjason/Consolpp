#include "Consolpp/Text/WrapIterator.hpp"

#include "Consolpp/Text/WrappedLine.hpp"

#include <iterator>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::WrapIterator;
    using Consolpp::Text::WrappedLine;

    TEST(WrapIteratorTest, YieldsTheFirstLine)
    {
        // Arrange
        constexpr WrappedLine expected{.start = 0, .end = 3, .width = 3};

        // Act
        const WrapIterator iterator{"aaa bbb", 3};

        // Assert
        EXPECT_EQ(*iterator, expected);
    }

    TEST(WrapIteratorTest, StartsTheNextLineAfterTheSpaces)
    {
        // Arrange
        WrapIterator iterator{"aaa bbb", 3};
        constexpr WrappedLine expected{.start = 4, .end = 7, .width = 3};

        // Act
        ++iterator;

        // Assert
        EXPECT_EQ(*iterator, expected);
    }

    TEST(WrapIteratorTest, EndsAfterTheLastLine)
    {
        // Arrange
        WrapIterator iterator{"aaa", 3};

        // Act
        ++iterator;

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }

    TEST(WrapIteratorTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr auto countLines = [] {
            int count = 0;

            for (WrapIterator iterator{"aa bb cc", 2}; iterator != std::default_sentinel; ++iterator) {
                ++count;
            }

            return count;
        };

        // Act
        constexpr int count = countLines();

        // Assert
        EXPECT_EQ(count, 3);
    }
} // namespace Consolpp::Tests::Unit::Text
