#include "Consolpp/Prompt/FindAction.hpp"

#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Keyboard/Modifiers.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/DefaultKeyBindings.hpp"

#include <optional>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Keyboard::Key;
    using Consolpp::Keyboard::KeyCode;
    using Consolpp::Keyboard::Modifiers;
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::defaultKeyBindings;
    using Consolpp::Prompt::findAction;

    TEST(FindActionTest, MatchesTheKeyWithItsModifiers)
    {
        // Arrange
        const Key key{.code = KeyCode::Character, .character = U'c', .modifiers = Modifiers{.control = true}};

        // Act
        const std::optional<Action> action = findAction(defaultKeyBindings, key);

        // Assert
        EXPECT_EQ(action, Action::Cancel);
    }

    TEST(FindActionTest, ConfirmsWithY)
    {
        // Arrange
        const Key key{.code = KeyCode::Character, .character = U'y'};

        // Act
        const std::optional<Action> action = findAction(defaultKeyBindings, key);

        // Assert
        EXPECT_EQ(action, Action::Confirm);
    }

    TEST(FindActionTest, FindsNothingForAnUnboundKey)
    {
        // Arrange
        const Key key{.code = KeyCode::Backspace, .modifiers = Modifiers{.alt = true}};

        // Act
        const std::optional<Action> action = findAction(defaultKeyBindings, key);

        // Assert
        EXPECT_FALSE(action.has_value());
    }

    TEST(FindActionTest, GoesToThePreviousFieldWithShiftTab)
    {
        // Arrange
        const Key key{.code = KeyCode::Tab, .modifiers = Modifiers{.shift = true}};

        // Act
        const std::optional<Action> action = findAction(defaultKeyBindings, key);

        // Assert
        EXPECT_EQ(action, Action::PreviousField);
    }
} // namespace Consolpp::Tests::Unit::Prompt
