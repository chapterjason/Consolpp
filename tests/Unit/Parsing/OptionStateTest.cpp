#include "Consolpp/Parsing/OptionState.hpp"

#include "Consolpp/Parsing/OptionPresence.hpp"

#include <cstddef>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    TEST(OptionStateTest, DefaultsToAbsentWithoutValue)
    {
        // Arrange
        constexpr Consolpp::Parsing::OptionPresence expected = Consolpp::Parsing::OptionPresence::Absent;

        // Act
        constexpr Consolpp::Parsing::OptionState state{};

        // Assert
        EXPECT_EQ(state.presence, expected);
        EXPECT_FALSE(state.valued);
        EXPECT_TRUE(state.value.empty());
        EXPECT_EQ(state.firstPosition, 0U);
    }

    TEST(OptionStateTest, FitsTheFirstPositionIntoThePaddingAfterTheValue)
    {
        // Arrange
        constexpr std::size_t expected = 24;

        // Act
        constexpr std::size_t size = sizeof(Consolpp::Parsing::OptionState);

        // Assert
        EXPECT_EQ(size, expected);
    }
} // namespace Consolpp::Tests::Unit::Parsing
