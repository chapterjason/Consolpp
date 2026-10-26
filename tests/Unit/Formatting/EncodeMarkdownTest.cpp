#include "Consolpp/Formatting/EncodeMarkdown.hpp"

#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::encodeMarkdown;

    TEST(EncodeMarkdownTest, EscapesTheCharactersMarkdownReads)
    {
        // Arrange
        constexpr std::string_view text = "a*b_c `d` [e] <f> #g |h ~i \\";
        std::string formatted;

        // Act
        encodeMarkdown(text, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "a\\*b\\_c \\`d\\` \\[e\\] \\<f\\> \\#g \\|h \\~i \\\\");
    }

    TEST(EncodeMarkdownTest, EscapesListMarkersAtTheStartOfALine)
    {
        // Arrange
        constexpr std::string_view text = "- a\n+ b\n1. c\nd - e 2.";
        std::string formatted;

        // Act
        encodeMarkdown(text, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "\\- a\n\\+ b\n1\\. c\nd - e 2.");
    }
} // namespace Consolpp::Tests::Unit::Formatting
