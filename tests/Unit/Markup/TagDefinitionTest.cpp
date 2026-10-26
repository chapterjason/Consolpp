#include "Consolpp/Markup/TagDefinition.hpp"

#include "Consolpp/Markup/Style.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Style;
    using Consolpp::Markup::TagDefinition;

    TEST(TagDefinitionTest, DiffersByStyle)
    {
        // Arrange
        constexpr TagDefinition first{.name = "a", .style = Style{}};
        constexpr TagDefinition second{.name = "a", .style = Style{.bold = true}};

        // Act
        constexpr bool equal = first == second;

        // Assert
        EXPECT_FALSE(equal);
    }
} // namespace Consolpp::Tests::Unit::Markup
