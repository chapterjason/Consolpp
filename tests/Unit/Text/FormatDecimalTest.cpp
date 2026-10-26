#include "Consolpp/Text/FormatDecimal.hpp"

#include <array>
#include <cstdint>
#include <iterator>
#include <limits>
#include <span>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::formatDecimal;

    TEST(FormatDecimalTest, WritesZero)
    {
        // Arrange
        std::string written;

        // Act
        formatDecimal(0, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "0");
    }

    TEST(FormatDecimalTest, WritesTheDigitsInOrder)
    {
        // Arrange
        constexpr std::uint32_t value = 255;
        std::string written;

        // Act
        formatDecimal(value, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "255");
    }

    TEST(FormatDecimalTest, WritesTheLargestValue)
    {
        // Arrange
        std::string written;

        // Act
        formatDecimal(std::numeric_limits<std::uint32_t>::max(), std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "4294967295");
    }

    TEST(FormatDecimalTest, ReturnsTheIteratorAfterTheLastDigit)
    {
        // Arrange
        constexpr std::uint32_t value = 38;
        std::array<char, 4> buffer{};
        const std::span<char> span{buffer};

        // Act
        const auto end = formatDecimal(value, span.begin());

        // Assert
        EXPECT_EQ(std::string_view(span.begin(), end), "38");
    }

    TEST(FormatDecimalTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr std::uint32_t value = 107;
        constexpr auto write = [] {
            std::array<char, 3> buffer{};

            formatDecimal(value, std::span<char>{buffer}.begin());

            return buffer;
        };

        // Act
        constexpr std::array<char, 3> buffer = write();

        // Assert
        EXPECT_EQ(std::string_view(buffer.data(), buffer.size()), "107");
    }
} // namespace Consolpp::Tests::Unit::Text
