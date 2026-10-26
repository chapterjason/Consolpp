#include "Consolpp/Styling/EncodeParsedMarkupSlice.hpp"

#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::ElementStyle;
    using Consolpp::Markup::ParsedMarkup;
    using Consolpp::Styling::encodeParsedMarkupSlice;
    using Consolpp::Styling::parseMarkup;
    using Consolpp::Terminal::Capabilities;

    TEST(EncodeParsedMarkupSliceTest, EncodesEveryRunInItsStyle)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = parseMarkup("a<info>b</info>c");
        std::string encoded;

        // Act
        encodeParsedMarkupSlice(markup.getSlice(0, 3), Capabilities{}, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "a\x1b[32mb\x1b[39mc");
    }

    TEST(EncodeParsedMarkupSliceTest, ClosesTheStyleInsideTheSlice)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = parseMarkup("<info>aaa bbb</info>");
        std::string encoded;

        // Act
        encodeParsedMarkupSlice(markup.getSlice(0, 3), Capabilities{}, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "\x1b[32maaa\x1b[39m");
    }

    TEST(EncodeParsedMarkupSliceTest, LeavesOutTheRunsOutsideTheSlice)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = parseMarkup("<info>a</info>b<comment>c</comment>");
        std::string encoded;

        // Act
        encodeParsedMarkupSlice(markup.getSlice(1, 2), Capabilities{}, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "b");
    }

    TEST(EncodeParsedMarkupSliceTest, ClosesTheLinkInsideTheSlice)
    {
        // Arrange
        const ParsedMarkup<ElementStyle> markup = parseMarkup(R"(<link href="u">abcd</link>)");
        std::string encoded;

        // Act
        encodeParsedMarkupSlice(markup.getSlice(2, 4), Capabilities{}, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "\x1b]8;;u\x1b\\cd\x1b]8;;\x1b\\");
    }
} // namespace Consolpp::Tests::Unit::Styling
