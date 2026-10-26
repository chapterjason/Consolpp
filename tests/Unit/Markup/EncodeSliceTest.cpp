#include "Consolpp/Markup/EncodeSlice.hpp"

#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/ResolveElement.hpp"

#include <cstddef>
#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::ElementStyle;
    using Consolpp::Markup::encodeSlice;
    using Consolpp::Markup::ParsedMarkup;
    using Consolpp::Markup::resolveElement;

    TEST(EncodeSliceTest, WritesATagAsAStyleElement)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = ParsedMarkup<ElementStyle>::fromMarkup("a<info>b</info>c",
                                                                                         ElementStyle{},
                                                                                         &resolveElement);
        std::string encoded;

        // Act
        encodeSlice(markup.getSlice(0, 3), std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, R"(a<style foreground="green">b</style>c)");
    }

    TEST(EncodeSliceTest, WritesOnlyThePartOfTheSlice)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = ParsedMarkup<ElementStyle>::fromMarkup("<info>aaa bbb</info>",
                                                                                         ElementStyle{},
                                                                                         &resolveElement);
        constexpr std::size_t start = 4;
        constexpr std::size_t end = 7;
        std::string encoded;

        // Act
        encodeSlice(markup.getSlice(start, end), std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, R"(<style foreground="green">bbb</style>)");
    }

    TEST(EncodeSliceTest, WritesTheColorsAndFlags)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = ParsedMarkup<ElementStyle>::fromMarkup(
            R"(<style background="#102030" bold="true" conceal="true">a</style>)",
            ElementStyle{},
            &resolveElement);
        std::string encoded;

        // Act
        encodeSlice(markup.getSlice(0, 1), std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, R"(<style background="#102030" bold="true" conceal="true">a</style>)");
    }

    TEST(EncodeSliceTest, WritesALinkWithItsStyle)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = ParsedMarkup<ElementStyle>::fromMarkup(
            R"(<link href="https://a.b/?c=1&amp;d=2" underline="true">site</link>)",
            ElementStyle{},
            &resolveElement);
        std::string encoded;

        // Act
        encodeSlice(markup.getSlice(0, 4), std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, R"(<link href="https://a.b/?c=1&amp;d=2" underline="true">site</link>)");
    }

    TEST(EncodeSliceTest, EscapesTheText)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = ParsedMarkup<ElementStyle>::fromMarkup("&lt;<info>&amp;</info>",
                                                                                         ElementStyle{},
                                                                                         &resolveElement);
        std::string encoded;

        // Act
        encodeSlice(markup.getSlice(0, 2), std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, R"(&lt;<style foreground="green">&amp;</style>)");
    }

    TEST(EncodeSliceTest, WritesNothingForAnEmptySlice)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = ParsedMarkup<ElementStyle>::fromMarkup("<info>a</info>",
                                                                                         ElementStyle{},
                                                                                         &resolveElement);
        std::string encoded;

        // Act
        encodeSlice(markup.getSlice(1, 1), std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "");
    }
} // namespace Consolpp::Tests::Unit::Markup
