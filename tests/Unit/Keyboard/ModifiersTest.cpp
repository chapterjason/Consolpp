#include "Consolpp/Keyboard/Modifiers.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Keyboard
{
    using Consolpp::Keyboard::Modifiers;

    TEST(ModifiersTest, DiffersByControl)
    {
        // Arrange
        constexpr Modifiers none{};
        constexpr Modifiers control{.control = true};

        // Act
        constexpr bool equal = none == control;

        // Assert
        EXPECT_FALSE(equal);
    }
} // namespace Consolpp::Tests::Unit::Keyboard
