#include "Consolpp/Formatting/FormatDefaultAsText.hpp"

#include <array>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatDefaultAsText;

    namespace
    {
        constexpr std::array<std::string_view, 2> values{"a", "<b>"};
    } // namespace

    TEST(FormatDefaultAsTextTest, FormatsTheDefaultValue)
    {
        // Arrange
        constexpr std::optional<std::string_view> defaultValue{"out"};
        constexpr std::span<const std::string_view> defaultValues{};
        std::string formatted;

        // Act
        formatDefaultAsText(defaultValue, defaultValues, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "<comment> [default: out]</comment>");
    }

    TEST(FormatDefaultAsTextTest, SeparatesAndEncodesTheDefaultValues)
    {
        // Arrange
        constexpr std::optional<std::string_view> defaultValue{};
        std::string formatted;

        // Act
        formatDefaultAsText(defaultValue, values, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "<comment> [default: a, &lt;b&gt;]</comment>");
    }
} // namespace Consolpp::Tests::Unit::Formatting
