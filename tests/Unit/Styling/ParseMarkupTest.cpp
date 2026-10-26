#include "Consolpp/Styling/ParseMarkup.hpp"

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/ElementError.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/Style.hpp"

#include <string>
#include <utility>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::ElementError;
    using Consolpp::Markup::ElementStyle;
    using Consolpp::Markup::ParsedMarkup;
    using Consolpp::Markup::Style;
    using Consolpp::Styling::parseMarkup;

    TEST(ParseMarkupTest, ResolvesTheStyleOfEveryRun)
    {
        // Act
        const ParsedMarkup<ElementStyle> parsed = parseMarkup("a<info>b</info>");

        // Assert
        ASSERT_EQ(parsed.getRuns().size(), 2U);
        EXPECT_EQ(parsed.getRuns().front().context.style, Style{});
        EXPECT_EQ(parsed.getRuns().back().context.style, Style::info);
    }

    TEST(ParseMarkupTest, KeepsTheHrefEscapedAndValidWhenMoved)
    {
        // Arrange
        std::string markup = R"(<link href="u?a=1&amp;b=2" foreground="green">x</link>)";
        ParsedMarkup<ElementStyle> original = parseMarkup(markup);

        markup.clear();

        // Act
        const ParsedMarkup<ElementStyle> moved = std::move(original);

        // Assert
        EXPECT_EQ(moved.getRuns().front().context.href, "u?a=1&amp;b=2");
        EXPECT_EQ(moved.getRuns().front().context.style, Style{.foreground = Color::green});
    }

    TEST(ParseMarkupTest, ThrowsForAnUnknownElement)
    {
        // Act
        const auto parse = [] {
            static_cast<void>(parseMarkup("<bogus>x</bogus>"));
        };

        // Assert
        EXPECT_THROW(parse(), ElementError);
    }
} // namespace Consolpp::Tests::Unit::Styling
