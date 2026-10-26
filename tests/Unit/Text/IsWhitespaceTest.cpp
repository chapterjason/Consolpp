#include "Consolpp/Text/IsWhitespace.hpp"

#include <algorithm>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::isWhitespace;

    TEST(IsWhitespaceTest, AcceptsSpaceTabLineFeedAndCarriageReturn)
    {
        // Arrange
        constexpr std::string_view characters = " \t\n\r";

        // Act
        const bool whitespace = std::ranges::all_of(characters, isWhitespace);

        // Assert
        EXPECT_TRUE(whitespace);
    }

    TEST(IsWhitespaceTest, RejectsOtherCharacters)
    {
        // Arrange
        constexpr std::string_view characters = "a0_\v\f";

        // Act
        const bool whitespace = std::ranges::any_of(characters, isWhitespace);

        // Assert
        EXPECT_FALSE(whitespace);
    }
} // namespace Consolpp::Tests::Unit::Text
