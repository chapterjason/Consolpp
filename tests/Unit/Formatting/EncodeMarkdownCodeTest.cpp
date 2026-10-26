#include "Consolpp/Formatting/EncodeMarkdownCode.hpp"

#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::encodeMarkdownCode;

    TEST(EncodeMarkdownCodeTest, WrapsTheTextInBackticks)
    {
        // Arrange
        constexpr std::string_view text = "--force";
        std::string formatted;

        // Act
        encodeMarkdownCode(text, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "`--force`");
    }

    TEST(EncodeMarkdownCodeTest, UsesMoreBackticksThanTheTextHolds)
    {
        // Arrange
        constexpr std::string_view text = "a``b";
        std::string formatted;

        // Act
        encodeMarkdownCode(text, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "```a``b```");
    }

    TEST(EncodeMarkdownCodeTest, PadsTextThatStartsWithABacktick)
    {
        // Arrange
        constexpr std::string_view text = "`a";
        std::string formatted;

        // Act
        encodeMarkdownCode(text, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "`` `a ``");
    }
} // namespace Consolpp::Tests::Unit::Formatting
