#include "Consolpp/Styling/EncodeMarkupSegment.hpp"

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::Style;
    using Consolpp::Styling::encodeMarkupSegment;
    using Consolpp::Terminal::Capabilities;

    TEST(EncodeMarkupSegmentTest, WritesUnstyledTextUnescaped)
    {
        // Arrange
        constexpr std::string_view text = "a &lt;b&gt;";
        std::string written;

        // Act
        encodeMarkupSegment(text, Style{}, std::string_view{}, Terminal::Capabilities{}, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "a <b>");
    }

    TEST(EncodeMarkupSegmentTest, WrapsTheTextInTheStyle)
    {
        // Arrange
        constexpr std::string_view text = "done";
        std::string written;

        // Act
        encodeMarkupSegment(text,
                            Style::info,
                            std::string_view{},
                            Terminal::Capabilities{},
                            std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[32mdone\x1b[39m");
    }

    TEST(EncodeMarkupSegmentTest, PutsTheLinkInsideTheColorsWithAnUnescapedHref)
    {
        // Arrange
        constexpr std::string_view text = "docs";
        constexpr Style style{.foreground = Color::green};
        constexpr std::string_view href = "https://example.com/?a=1&amp;b=2";
        std::string written;

        // Act
        encodeMarkupSegment(text, style, href, Terminal::Capabilities{}, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[32m\x1b]8;;https://example.com/?a=1&b=2\x1b\\docs\x1b]8;;\x1b\\\x1b[39m");
    }

    TEST(EncodeMarkupSegmentTest, LeavesTheLinkOutWithoutHyperlinks)
    {
        // Arrange
        constexpr std::string_view text = "docs";
        constexpr Style style{.foreground = Color::green};
        constexpr std::string_view href = "https://example.com";
        constexpr Terminal::Capabilities capabilities{.hyperlinks = false};
        std::string written;

        // Act
        encodeMarkupSegment(text, style, href, capabilities, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[32mdocs\x1b[39m");
    }
} // namespace Consolpp::Tests::Unit::Styling
