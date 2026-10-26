#include "Consolpp/Markup/Attribute.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Attribute;

    TEST(AttributeTest, DiffersByValuePosition)
    {
        // Arrange
        constexpr Attribute first{.name = "a", .namePosition = 1, .value = "1", .valuePosition = 4};
        constexpr Attribute second{.name = "a", .namePosition = 1, .value = "1", .valuePosition = 5};

        // Act
        constexpr bool equal = first == second;

        // Assert
        EXPECT_FALSE(equal);
    }
} // namespace Consolpp::Tests::Unit::Markup
