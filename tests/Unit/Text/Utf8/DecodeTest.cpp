#include "Consolpp/Text/Utf8/Decode.hpp"

#include "Consolpp/Text/Utf8/CodePoint.hpp"

#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Utf8
{
    using Consolpp::Text::Utf8::CodePoint;
    using Consolpp::Text::Utf8::decode;

    TEST(DecodeTest, DecodesAscii)
    {
        // Arrange
        constexpr std::string_view text = "A";
        constexpr CodePoint expected{.value = 0x41, .length = 1};

        // Act
        const CodePoint codePoint = decode(text);

        // Assert
        EXPECT_EQ(codePoint, expected);
    }

    TEST(DecodeTest, DecodesTwoBytes)
    {
        // Arrange
        constexpr std::string_view text = "é";
        constexpr CodePoint expected{.value = 0xE9, .length = 2};

        // Act
        const CodePoint codePoint = decode(text);

        // Assert
        EXPECT_EQ(codePoint, expected);
    }

    TEST(DecodeTest, DecodesThreeBytes)
    {
        // Arrange
        constexpr std::string_view text = "€";
        constexpr CodePoint expected{.value = 0x20AC, .length = 3};

        // Act
        const CodePoint codePoint = decode(text);

        // Assert
        EXPECT_EQ(codePoint, expected);
    }

    TEST(DecodeTest, DecodesFourBytes)
    {
        // Arrange
        constexpr std::string_view text = "😀";
        constexpr CodePoint expected{.value = 0x1F600, .length = 4};

        // Act
        const CodePoint codePoint = decode(text);

        // Assert
        EXPECT_EQ(codePoint, expected);
    }

    TEST(DecodeTest, DecodesOnlyTheFirstCodePoint)
    {
        // Arrange
        constexpr std::string_view text = "éa";
        constexpr CodePoint expected{.value = 0xE9, .length = 2};

        // Act
        const CodePoint codePoint = decode(text);

        // Assert
        EXPECT_EQ(codePoint, expected);
    }

    TEST(DecodeTest, DecodesTheLastCodePoint)
    {
        // Arrange
        constexpr std::string_view text = "\xF4\x8F\xBF\xBF";
        constexpr CodePoint expected{.value = 0x10FFFF, .length = 4};

        // Act
        const CodePoint codePoint = decode(text);

        // Assert
        EXPECT_EQ(codePoint, expected);
    }

    TEST(DecodeTest, ThrowsForAnEmptyText)
    {
        // Arrange
        constexpr std::string_view text{};
        bool thrown = false;

        // Act
        try {
            static_cast<void>(decode(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(DecodeTest, ThrowsForALoneContinuationByte)
    {
        // Arrange
        constexpr std::string_view text = "\x80";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(decode(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(DecodeTest, ThrowsForATruncatedSequence)
    {
        // Arrange
        constexpr std::string_view text = "\xE2\x82";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(decode(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(DecodeTest, ThrowsForAMissingContinuationByte)
    {
        // Arrange
        constexpr std::string_view text = "\xC3"
                                          "A";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(decode(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(DecodeTest, ThrowsForAnOverlongForm)
    {
        // Arrange
        constexpr std::string_view text = "\xC0\xAF";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(decode(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(DecodeTest, ThrowsForASurrogate)
    {
        // Arrange
        constexpr std::string_view text = "\xED\xA0\x80";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(decode(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(DecodeTest, ThrowsBeyondTheLastCodePoint)
    {
        // Arrange
        constexpr std::string_view text = "\xF4\x90\x80\x80";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(decode(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(DecodeTest, ThrowsForAnInvalidLeadingByte)
    {
        // Arrange
        constexpr std::string_view text = "\xFF";
        bool thrown = false;

        // Act
        try {
            static_cast<void>(decode(text));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }
} // namespace Consolpp::Tests::Unit::Text::Utf8
