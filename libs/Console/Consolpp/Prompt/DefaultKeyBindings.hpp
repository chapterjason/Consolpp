#pragma once

#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Keyboard/Modifiers.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/KeyBinding.hpp"

#include <array>

namespace Consolpp::Prompt
{
    inline constexpr std::array<KeyBinding, 18> defaultKeyBindings{
        {
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Left}, .action = Action::MoveLeft},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Right}, .action = Action::MoveRight},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Up}, .action = Action::MoveUp},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Down}, .action = Action::MoveDown},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Home}, .action = Action::MoveToStart},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::End}, .action = Action::MoveToEnd},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Backspace}, .action = Action::EraseBackward},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Delete}, .action = Action::EraseForward},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Tab}, .action = Action::Complete},
            KeyBinding{
                .key = Keyboard::Key{.code = Keyboard::KeyCode::Character, .character = U' '},
                .action = Action::Toggle,
            },
            KeyBinding{
                .key = Keyboard::Key{.code = Keyboard::KeyCode::Character, .character = U'y'},
                .action = Action::Confirm,
            },
            KeyBinding{
                .key = Keyboard::Key{.code = Keyboard::KeyCode::Character, .character = U'Y'},
                .action = Action::Confirm,
            },
            KeyBinding{
                .key = Keyboard::Key{.code = Keyboard::KeyCode::Character, .character = U'n'},
                .action = Action::Deny,
            },
            KeyBinding{
                .key = Keyboard::Key{.code = Keyboard::KeyCode::Character, .character = U'N'},
                .action = Action::Deny,
            },
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Enter}, .action = Action::Submit},
            KeyBinding{.key = Keyboard::Key{.code = Keyboard::KeyCode::Escape}, .action = Action::Cancel},
            KeyBinding{
                .key =
                    Keyboard::Key{
                        .code = Keyboard::KeyCode::Character,
                        .character = U'c',
                        .modifiers = Keyboard::Modifiers{.control = true},
                    },
                .action = Action::Cancel,
            },
            KeyBinding{
                .key = Keyboard::Key{.code = Keyboard::KeyCode::Tab, .modifiers = Keyboard::Modifiers{.shift = true}},
                .action = Action::PreviousField,
            },
        },
    };
} // namespace Consolpp::Prompt
