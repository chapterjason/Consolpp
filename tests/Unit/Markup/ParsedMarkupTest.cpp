#include "Consolpp/Markup/ParsedMarkup.hpp"

#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/ParseError.hpp"
#include "Consolpp/Markup/ParsedMarkupSlice.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Element;
    using Consolpp::Markup::ParsedMarkup;
    using Consolpp::Markup::ParsedMarkupSlice;
    using Consolpp::Markup::ParseError;

    namespace
    {
        constexpr std::string_view getName(const Element& element)
        {
            return element.name;
        }
    } // namespace

    TEST(ParsedMarkupTest, KeepsTheVisibleTextWithDecodedEntities)
    {
        // Arrange
        constexpr std::string_view markup = "a<x>b &amp; c</x>";

        // Act
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup(markup, "", getName);

        // Assert
        EXPECT_EQ(parsed.getText(), "ab & c");
    }

    TEST(ParsedMarkupTest, KeepsARunWithTheContextOfEveryText)
    {
        // Arrange
        constexpr std::string_view markup = "a<x>b &amp; c</x>";

        // Act
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup(markup, "", getName);

        // Assert
        ASSERT_EQ(parsed.getRuns().size(), 2U);
        EXPECT_EQ(parsed.getRuns().front().start, 0U);
        EXPECT_EQ(parsed.getRuns().front().length, 1U);
        EXPECT_EQ(parsed.getRuns().front().context, "");
        EXPECT_EQ(parsed.getRuns().back().start, 1U);
        EXPECT_EQ(parsed.getRuns().back().length, 5U);
        EXPECT_EQ(parsed.getRuns().back().context, "x");
    }

    TEST(ParsedMarkupTest, KeepsContextsThatPointIntoTheMarkupValidWhenMoved)
    {
        // Arrange
        std::string markup = "<x>a</x>";
        ParsedMarkup<std::string_view> original = ParsedMarkup<std::string_view>::fromMarkup(markup, "", getName);

        markup.assign("<y>b</y>");

        // Act
        const ParsedMarkup<std::string_view> moved = std::move(original);

        // Assert
        EXPECT_EQ(moved.getRuns().front().context, "x");
    }

    TEST(ParsedMarkupTest, SplitLinesCutsTheTextAndTheRunsAtLineBreaks)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("a<x>b\nc</x>d",
                                                                                                 "",
                                                                                                 getName);

        // Act
        const std::vector<ParsedMarkup<std::string_view>> lines = parsed.splitLines();

        // Assert
        ASSERT_EQ(lines.size(), 2U);
        EXPECT_EQ(lines.front().getText(), "ab");
        EXPECT_EQ(lines.back().getText(), "cd");
        ASSERT_EQ(lines.back().getRuns().size(), 2U);
        EXPECT_EQ(lines.back().getRuns().front().length, 1U);
        EXPECT_EQ(lines.back().getRuns().front().context, "x");
        EXPECT_EQ(lines.back().getRuns().back().start, 1U);
        EXPECT_EQ(lines.back().getRuns().back().context, "");
    }

    TEST(ParsedMarkupTest, SplitLinesKeepsOneEmptyLineForAnEmptyText)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("", "", getName);

        // Act
        const std::vector<ParsedMarkup<std::string_view>> lines = parsed.splitLines();

        // Assert
        ASSERT_EQ(lines.size(), 1U);
        EXPECT_TRUE(lines.front().getText().empty());
    }

    TEST(ParsedMarkupTest, GetSliceKeepsTheMarkupAndTheRange)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("abc", "", getName);

        // Act
        const ParsedMarkupSlice<std::string_view> slice = parsed.getSlice(1, 2);

        // Assert
        EXPECT_EQ(slice.markup, &parsed);
        EXPECT_EQ(slice.start, 1U);
        EXPECT_EQ(slice.end, 2U);
    }

    TEST(ParsedMarkupTest, GetSliceThrowsForARangeOutsideTheText)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("abc", "", getName);

        // Act
        const auto getSlice = [&parsed] {
            static_cast<void>(parsed.getSlice(2, 4));
        };

        // Assert
        EXPECT_THROW(getSlice(), std::out_of_range);
    }

    TEST(ParsedMarkupTest, GetLinesWrapsTheVisibleText)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("<x>aa</x> bb",
                                                                                                 "",
                                                                                                 getName);
        const std::vector<std::size_t> expected{0, 3};
        std::vector<std::size_t> starts;

        // Act
        for (const ParsedMarkupSlice<std::string_view> line : parsed.getLines(2)) {
            starts.push_back(line.start);
        }

        // Assert
        EXPECT_EQ(starts, expected);
    }

    TEST(ParsedMarkupTest, ThrowsForInvalidMarkup)
    {
        // Act
        const auto parse = [] {
            static_cast<void>(ParsedMarkup<std::string_view>::fromMarkup("<x>broken", "", getName));
        };

        // Assert
        EXPECT_THROW(parse(), ParseError);
    }
} // namespace Consolpp::Tests::Unit::Markup
