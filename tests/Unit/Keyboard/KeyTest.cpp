#include "Consolpp/Keyboard/Key.hpp"

#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Keyboard/Modifiers.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Keyboard
{
    using Consolpp::Keyboard::Key;
    using Consolpp::Keyboard::KeyCode;
    using Consolpp::Keyboard::Modifiers;

    TEST(KeyTest, HasNoCharacterAndNoModifiersByDefault)
    {
        // Act
        constexpr Key key{.code = KeyCode::Enter};

        // Assert
        EXPECT_EQ(key.character, U'\0');
        EXPECT_EQ(key.modifiers, Modifiers{});
    }
} // namespace Consolpp::Tests::Unit::Keyboard
