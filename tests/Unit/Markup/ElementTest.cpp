#include "Consolpp/Markup/Element.hpp"

#include "Consolpp/Markup/Attribute.hpp"
#include "Consolpp/Markup/AttributeRange.hpp"

#include <cstddef>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Attribute;
    using Consolpp::Markup::AttributeRange;
    using Consolpp::Markup::Element;

    TEST(ElementTest, GetAttributesStartsAtTheAttributesPosition)
    {
        // Arrange
        constexpr Element element{
            .name = "x",
            .position = 4,
            .attributes = R"( a="1")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        const std::vector<std::size_t> expected{7};
        std::vector<std::size_t> positions;

        // Act
        const AttributeRange attributes = element.getAttributes();

        // Assert
        for (const Attribute& attribute : attributes) {
            positions.push_back(attribute.namePosition);
        }

        EXPECT_EQ(positions, expected);
    }
} // namespace Consolpp::Tests::Unit::Markup
