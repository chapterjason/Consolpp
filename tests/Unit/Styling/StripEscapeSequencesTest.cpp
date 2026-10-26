#include "Consolpp/Styling/StripEscapeSequences.hpp"

#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Styling::stripEscapeSequences;

    TEST(StripEscapeSequencesTest, KeepsPlainText)
    {
        // Arrange
        constexpr std::string_view text = "plain text";
        std::string written;

        // Act
        stripEscapeSequences(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "plain text");
    }

    TEST(StripEscapeSequencesTest, RemovesStyles)
    {
        // Arrange
        constexpr std::string_view text = "a\x1b[32;1mok\x1b[39;22mb";
        std::string written;

        // Act
        stripEscapeSequences(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "aokb");
    }

    TEST(StripEscapeSequencesTest, RemovesCursorMoves)
    {
        // Arrange
        constexpr std::string_view text = "\x1b[2A\x1b[0Jx";
        std::string written;

        // Act
        stripEscapeSequences(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "x");
    }

    TEST(StripEscapeSequencesTest, RemovesALinkEndedByTheStringTerminator)
    {
        // Arrange
        constexpr std::string_view text = "\x1b]8;;https://example.com\x1b\\docs\x1b]8;;\x1b\\";
        std::string written;

        // Act
        stripEscapeSequences(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "docs");
    }

    TEST(StripEscapeSequencesTest, RemovesATitleEndedByBell)
    {
        // Arrange
        constexpr std::string_view text = "\x1b]0;title\x07x";
        std::string written;

        // Act
        stripEscapeSequences(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "x");
    }

    TEST(StripEscapeSequencesTest, KeepsAnIncompleteControlSequence)
    {
        // Arrange
        constexpr std::string_view text = "a\x1b[32";
        std::string written;

        // Act
        stripEscapeSequences(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "a\x1b[32");
    }

    TEST(StripEscapeSequencesTest, KeepsAnIncompleteOperatingSystemCommand)
    {
        // Arrange
        constexpr std::string_view text = "\x1b]8;;https://example.com";
        std::string written;

        // Act
        stripEscapeSequences(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b]8;;https://example.com");
    }

    TEST(StripEscapeSequencesTest, KeepsALoneEscape)
    {
        // Arrange
        constexpr std::string_view text = "a\x1b";
        std::string written;

        // Act
        stripEscapeSequences(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "a\x1b");
    }
} // namespace Consolpp::Tests::Unit::Styling
