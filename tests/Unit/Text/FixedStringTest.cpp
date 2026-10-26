#include "Consolpp/Text/FixedString.hpp"

#include <cstddef>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    TEST(FixedStringTest, GetViewReturnsTheTextWithoutTerminator)
    {
        // Arrange
        constexpr Consolpp::Text::FixedString text{"verbose"};

        // Act
        const std::string_view view = text.getView();

        // Assert
        EXPECT_EQ(view, "verbose");
        EXPECT_EQ(view.size(), 7U);
    }

    TEST(FixedStringTest, GetViewReturnsAnEmptyViewForAnEmptyLiteral)
    {
        // Arrange
        constexpr Consolpp::Text::FixedString text{""};

        // Act
        const std::string_view view = text.getView();

        // Assert
        EXPECT_TRUE(view.empty());
    }

    TEST(FixedStringTest, StoresOnlyTheCharactersOfTheLiteral)
    {
        // Arrange
        constexpr Consolpp::Text::FixedString text{"tag"};

        // Act
        constexpr std::size_t size = text.characters.size();

        // Assert
        EXPECT_EQ(size, 3U);
    }
} // namespace Consolpp::Tests::Unit::Text
