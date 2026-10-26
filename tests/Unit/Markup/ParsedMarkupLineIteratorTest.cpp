#include "Consolpp/Markup/ParsedMarkupLineIterator.hpp"

#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/ParsedMarkupSlice.hpp"
#include "Consolpp/Text/WrapIterator.hpp"

#include <iterator>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Element;
    using Consolpp::Markup::ParsedMarkup;
    using Consolpp::Markup::ParsedMarkupLineIterator;
    using Consolpp::Markup::ParsedMarkupSlice;
    using Consolpp::Text::WrapIterator;

    namespace
    {
        constexpr std::string_view getName(const Element& element)
        {
            return element.name;
        }
    } // namespace

    TEST(ParsedMarkupLineIteratorTest, YieldsTheLineAsASliceOfTheMarkup)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("aaa bbb",
                                                                                                 "",
                                                                                                 getName);
        ParsedMarkupLineIterator<std::string_view> iterator{&parsed, WrapIterator{parsed.getText(), 3}};

        // Act
        ++iterator;

        // Assert
        const ParsedMarkupSlice<std::string_view> slice = *iterator;

        EXPECT_EQ(slice.markup, &parsed);
        EXPECT_EQ(slice.start, 4U);
        EXPECT_EQ(slice.end, 7U);
    }

    TEST(ParsedMarkupLineIteratorTest, EndsWithTheLines)
    {
        // Arrange
        const ParsedMarkup<std::string_view> parsed = ParsedMarkup<std::string_view>::fromMarkup("aaa", "", getName);
        ParsedMarkupLineIterator<std::string_view> iterator{&parsed, WrapIterator{parsed.getText(), 3}};

        // Act
        ++iterator;

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }
} // namespace Consolpp::Tests::Unit::Markup
