#include "Consolpp/Parsing/VariadicState.hpp"

#include <cstddef>
#include <cstdint>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    TEST(VariadicStateTest, DefaultsToTheFirstPositionBeforeTheSeparator)
    {
        // Arrange
        constexpr std::uint32_t expected = 0;

        // Act
        constexpr Consolpp::Parsing::VariadicState state{};

        // Assert
        EXPECT_EQ(state.firstPosition, expected);
        EXPECT_FALSE(state.afterSeparator);
    }

    TEST(VariadicStateTest, TakesEightBytes)
    {
        // Arrange
        constexpr std::size_t expected = 8;

        // Act
        constexpr std::size_t size = sizeof(Consolpp::Parsing::VariadicState);

        // Assert
        EXPECT_EQ(size, expected);
    }
} // namespace Consolpp::Tests::Unit::Parsing
