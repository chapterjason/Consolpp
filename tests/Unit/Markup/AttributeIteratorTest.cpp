#include "Consolpp/Markup/AttributeIterator.hpp"

#include "Consolpp/Markup/Attribute.hpp"

#include <cstddef>
#include <iterator>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Attribute;
    using Consolpp::Markup::AttributeIterator;

    TEST(AttributeIteratorTest, YieldsTheFirstAttribute)
    {
        // Arrange
        constexpr std::size_t position = 10;
        constexpr Attribute expected{.name = "a", .namePosition = 11, .value = "1", .valuePosition = 16};

        // Act
        const AttributeIterator iterator{R"( a = "1" b='2')", position};

        // Assert
        EXPECT_EQ(*iterator, expected);
    }

    TEST(AttributeIteratorTest, MovesToTheNextAttribute)
    {
        // Arrange
        constexpr std::size_t position = 10;
        AttributeIterator iterator{R"( a = "1" b='2')", position};
        constexpr Attribute expected{.name = "b", .namePosition = 19, .value = "2", .valuePosition = 22};

        // Act
        ++iterator;

        // Assert
        EXPECT_EQ(*iterator, expected);
    }

    TEST(AttributeIteratorTest, EndsAfterTheLastAttribute)
    {
        // Arrange
        AttributeIterator iterator{R"( a="1" )", 0};

        // Act
        ++iterator;

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }

    TEST(AttributeIteratorTest, EndsRightAwayWithoutAttributes)
    {
        // Act
        const AttributeIterator iterator{"  ", 0};

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }
} // namespace Consolpp::Tests::Unit::Markup
