#include "Consolpp/Markup/ElementStyle.hpp"

#include "Consolpp/Markup/Style.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::ElementStyle;
    using Consolpp::Markup::Style;

    TEST(ElementStyleTest, DefaultsToNoStyleAndNoLink)
    {
        // Act
        constexpr ElementStyle element{};

        // Assert
        EXPECT_EQ(element.style, Style{});
        EXPECT_TRUE(element.href.empty());
    }
} // namespace Consolpp::Tests::Unit::Markup
