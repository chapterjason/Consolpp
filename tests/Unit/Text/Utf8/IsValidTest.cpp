#include "Consolpp/Text/Utf8/IsValid.hpp"

#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Utf8
{
    using Consolpp::Text::Utf8::isValid;

    TEST(IsValidTest, AcceptsEmptyText)
    {
        // Arrange
        constexpr std::string_view text{};

        // Act
        const bool valid = isValid(text);

        // Assert
        EXPECT_TRUE(valid);
    }

    TEST(IsValidTest, AcceptsMultibyteText)
    {
        // Arrange
        constexpr std::string_view text = "a\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x80";

        // Act
        const bool valid = isValid(text);

        // Assert
        EXPECT_TRUE(valid);
    }

    TEST(IsValidTest, RejectsATruncatedSequence)
    {
        // Arrange
        constexpr std::string_view text = "a\xC3";

        // Act
        const bool valid = isValid(text);

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(IsValidTest, RejectsAnOverlongForm)
    {
        // Arrange
        constexpr std::string_view text = "\xC0\x80";

        // Act
        const bool valid = isValid(text);

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(IsValidTest, RejectsASurrogate)
    {
        // Arrange
        constexpr std::string_view text = "\xED\xA0\x80";

        // Act
        const bool valid = isValid(text);

        // Assert
        EXPECT_FALSE(valid);
    }
} // namespace Consolpp::Tests::Unit::Text::Utf8
