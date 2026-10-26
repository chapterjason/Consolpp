#include "Consolpp/Formatting/EncodeJsonArray.hpp"

#include <array>
#include <iterator>
#include <span>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::encodeJsonArray;

    TEST(EncodeJsonArrayTest, WritesTheStringsWithoutSpaces)
    {
        // Arrange
        constexpr std::array<std::string_view, 2> values{"a", "b\"c"};
        std::string written;

        // Act
        encodeJsonArray(values, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, R"(["a","b\"c"])");
    }

    TEST(EncodeJsonArrayTest, WritesAnEmptyArray)
    {
        // Arrange
        constexpr std::span<const std::string_view> values{};
        std::string written;

        // Act
        encodeJsonArray(values, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "[]");
    }
} // namespace Consolpp::Tests::Unit::Formatting
