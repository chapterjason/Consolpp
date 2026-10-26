#include "Consolpp/Markup/IsName.hpp"

#include <algorithm>
#include <array>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::isName;

    TEST(IsNameTest, AcceptsLowercaseLettersWithSingleHyphensBetween)
    {
        // Arrange
        constexpr std::array<std::string_view, 3> names{"info", "a-b", "any-tag-name"};

        // Act
        const bool accepted = std::ranges::all_of(names, isName);

        // Assert
        EXPECT_TRUE(accepted);
    }

    TEST(IsNameTest, RejectsEverythingElse)
    {
        // Arrange
        constexpr std::array<std::string_view, 7> names{"", "Info", "a1", "a_b", "-a", "a-", "a--b"};

        // Act
        const bool accepted = std::ranges::any_of(names, isName);

        // Assert
        EXPECT_FALSE(accepted);
    }
} // namespace Consolpp::Tests::Unit::Markup
