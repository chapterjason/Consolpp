#include "Consolpp/Formatting/EncodeJsonString.hpp"

#include <iterator>
#include <optional>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::encodeJsonString;

    TEST(EncodeJsonStringTest, WritesPlainTextInQuotes)
    {
        // Arrange
        constexpr std::string_view text = "deploy";
        std::string written;

        // Act
        encodeJsonString(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, R"("deploy")");
    }

    TEST(EncodeJsonStringTest, EscapesQuotesAndBackslashes)
    {
        // Arrange
        constexpr std::string_view text = R"(a"b\c)";
        std::string written;

        // Act
        encodeJsonString(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, R"("a\"b\\c")");
    }

    TEST(EncodeJsonStringTest, EscapesNamedControlCharacters)
    {
        // Arrange
        constexpr std::string_view text = "\b\f\n\r\t";
        std::string written;

        // Act
        encodeJsonString(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, R"("\b\f\n\r\t")");
    }

    TEST(EncodeJsonStringTest, EscapesOtherControlCharactersAsUnicode)
    {
        // Arrange
        constexpr std::string_view text = "\x01\x1f";
        std::string written;

        // Act
        encodeJsonString(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, R"("\u0001\u001f")");
    }

    TEST(EncodeJsonStringTest, KeepsSlashesAndNonAscii)
    {
        // Arrange
        constexpr std::string_view text = "a/b \u00E9";
        std::string written;

        // Act
        encodeJsonString(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\"a/b \u00E9\"");
    }

    TEST(EncodeJsonStringTest, EncodesAMissingValueAsNull)
    {
        // Arrange
        constexpr std::optional<std::string_view> text{};
        std::string written;

        // Act
        encodeJsonString(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "null");
    }

    TEST(EncodeJsonStringTest, EncodesAPresentValueAsString)
    {
        // Arrange
        constexpr std::optional<std::string_view> text{"out"};
        std::string written;

        // Act
        encodeJsonString(text, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, R"("out")");
    }
} // namespace Consolpp::Tests::Unit::Formatting
