#include "Consolpp/Text/Utf8/Encode.hpp"

#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Utf8
{
    using Consolpp::Text::Utf8::encode;

    TEST(EncodeTest, WritesAsciiAsOneByte)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x41;
        std::string written;

        // Act
        encode(codePoint, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "A");
    }

    TEST(EncodeTest, WritesTwoBytes)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0xE9;
        std::string written;

        // Act
        encode(codePoint, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "é");
    }

    TEST(EncodeTest, WritesThreeBytes)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x20AC;
        std::string written;

        // Act
        encode(codePoint, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "€");
    }

    TEST(EncodeTest, WritesFourBytes)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x1F600;
        std::string written;

        // Act
        encode(codePoint, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "😀");
    }

    TEST(EncodeTest, WritesTheLastCodePoint)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x10FFFF;
        std::string written;

        // Act
        encode(codePoint, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\xF4\x8F\xBF\xBF");
    }

    TEST(EncodeTest, WritesZero)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x0;
        std::string written;

        // Act
        encode(codePoint, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, std::string_view("\0", 1));
    }

    TEST(EncodeTest, ThrowsForASurrogate)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0xD800;
        std::string written;
        bool thrown = false;

        // Act
        try {
            encode(codePoint, std::back_inserter(written));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(EncodeTest, ThrowsBeyondTheLastCodePoint)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x110000;
        std::string written;
        bool thrown = false;

        // Act
        try {
            encode(codePoint, std::back_inserter(written));
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }
} // namespace Consolpp::Tests::Unit::Text::Utf8
