#include "Consolpp/Markup/AttributeRange.hpp"

#include "Consolpp/Markup/Attribute.hpp"

#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Attribute;
    using Consolpp::Markup::AttributeRange;

    TEST(AttributeRangeTest, IteratesEveryAttribute)
    {
        // Arrange
        constexpr AttributeRange range{R"( a="1" b="2" c="3")", 0};
        const std::vector<std::string> expected{"a", "b", "c"};
        std::vector<std::string> names;

        // Act
        for (const Attribute& attribute : range) {
            names.emplace_back(attribute.name);
        }

        // Assert
        EXPECT_EQ(names, expected);
    }
} // namespace Consolpp::Tests::Unit::Markup
