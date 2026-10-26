#include "Consolpp/Markup/ParsedMarkupLineRange.hpp"

#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/ParsedMarkupSlice.hpp"

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Element;
    using Consolpp::Markup::ParsedMarkup;
    using Consolpp::Markup::ParsedMarkupLineRange;
    using Consolpp::Markup::ParsedMarkupSlice;

    namespace
    {
        constexpr std::string_view getName(const Element& element)
        {
            return element.name;
        }
    } // namespace

    TEST(ParsedMarkupLineRangeTest, YieldsEveryWrappedLine)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("<x>aaa bbb</x> cc",
                                                                                                 "",
                                                                                                 getName);
        const ParsedMarkupLineRange<std::string_view> range{&parsed, parsed.getText(), 3};
        const std::vector<std::string> expected{"aaa", "bbb", "cc"};
        std::vector<std::string> lines;

        // Act
        for (const ParsedMarkupSlice<std::string_view> line : range) {
            lines.emplace_back(parsed.getText().substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(ParsedMarkupLineRangeTest, ThrowsForAWidthOfZero)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("a", "", getName);
        const ParsedMarkupLineRange<std::string_view> range{&parsed, parsed.getText(), 0};

        // Act
        const auto begin = [&range] {
            static_cast<void>(range.begin());
        };

        // Assert
        EXPECT_THROW(begin(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Markup
