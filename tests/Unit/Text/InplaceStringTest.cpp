#include "Consolpp/Text/InplaceString.hpp"

#include <iterator>
#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::InplaceString;

    TEST(InplaceStringTest, IsEmptyAtFirst)
    {
        // Act
        const InplaceString<4> text;

        // Assert
        EXPECT_TRUE(text.getView().empty());
    }

    TEST(InplaceStringTest, KeepsTheAddedCharacters)
    {
        // Arrange
        InplaceString<4> text;
        constexpr std::string_view characters = "abcd";

        // Act
        for (const char character : characters) {
            text.push_back(character);
        }

        // Assert
        EXPECT_EQ(text.getView(), "abcd");
    }

    TEST(InplaceStringTest, ThrowsWhenTheCapacityIsReached)
    {
        // Arrange
        InplaceString<1> text;

        text.push_back('a');

        // Act
        const auto addCharacter = [&text] {
            text.push_back('b');
        };

        // Assert
        EXPECT_THROW(addCharacter(), std::length_error);
    }

    TEST(InplaceStringTest, WorksWithABackInserter)
    {
        // Arrange
        InplaceString<4> text;
        auto inserter = std::back_inserter(text);

        // Act
        *inserter = 'x';

        // Assert
        EXPECT_EQ(text.getView(), "x");
    }
} // namespace Consolpp::Tests::Unit::Text
