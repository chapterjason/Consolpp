#include "Consolpp/Styling/StyledSpan.hpp"

#include "Consolpp/Markup/Style.hpp"

#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::Style;
    using Consolpp::Styling::StyledSpan;

    TEST(StyledSpanTest, KeepsTheStyleAndTheText)
    {
        // Arrange
        constexpr std::string_view text = "done";

        // Act
        constexpr StyledSpan styled{.style = Style::info, .text = text};

        // Assert
        EXPECT_EQ(styled.style, Style::info);
        EXPECT_EQ(styled.text, "done");
    }
} // namespace Consolpp::Tests::Unit::Styling
