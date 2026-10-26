#include "Consolpp/Styling/EncodeMarkup.hpp"

#include "Consolpp/Markup/ParseError.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::ParseError;
    using Consolpp::Styling::encodeMarkup;
    using Consolpp::Terminal::Capabilities;

    TEST(EncodeMarkupTest, StylesEveryTextBetweenTags)
    {
        // Arrange
        constexpr std::string_view markup = "a<info>b</info>c";
        std::string written;

        // Act
        encodeMarkup(markup, Terminal::Capabilities{}, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "a\x1b[32mb\x1b[39mc");
    }

    TEST(EncodeMarkupTest, StylesNestedTextWithTheInnermostStyleOnly)
    {
        // Arrange
        constexpr std::string_view markup = "<info>a<comment>b</comment>c</info>";
        std::string written;

        // Act
        encodeMarkup(markup, Terminal::Capabilities{}, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[32ma\x1b[39m\x1b[33mb\x1b[39m\x1b[32mc\x1b[39m");
    }

    TEST(EncodeMarkupTest, ResolvesEntities)
    {
        // Arrange
        constexpr std::string_view markup = "<info>&lt;ok&gt;</info>";
        std::string written;

        // Act
        encodeMarkup(markup, Terminal::Capabilities{}, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[32m<ok>\x1b[39m");
    }

    TEST(EncodeMarkupTest, WritesNothingForInvalidMarkup)
    {
        // Arrange
        constexpr std::string_view markup = "fine <info>broken</comment>";
        std::string written;
        bool thrown = false;

        // Act
        try {
            encodeMarkup(markup, Terminal::Capabilities{}, std::back_inserter(written));
        } catch (const ParseError&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
        EXPECT_TRUE(written.empty());
    }
} // namespace Consolpp::Tests::Unit::Styling
