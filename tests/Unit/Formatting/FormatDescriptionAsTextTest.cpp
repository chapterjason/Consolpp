#include "Consolpp/Formatting/FormatDescriptionAsText.hpp"

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatDescriptionAsText;

    TEST(FormatDescriptionAsTextTest, IndentsAndTrimsTheFollowingLines)
    {
        // Arrange
        constexpr std::string_view description = "first  \n   second\t\n third";
        constexpr std::size_t indent = 4;
        std::string formatted;

        // Act
        formatDescriptionAsText(description, indent, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "first\n    second\n    third");
    }

    TEST(FormatDescriptionAsTextTest, KeepsTheMarkup)
    {
        // Arrange
        constexpr std::string_view description = "a <info>b</info> &amp; c";
        constexpr std::size_t indent = 0;
        std::string formatted;

        // Act
        formatDescriptionAsText(description, indent, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "a <info>b</info> &amp; c");
    }
} // namespace Consolpp::Tests::Unit::Formatting
