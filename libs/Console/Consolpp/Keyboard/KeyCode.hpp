#pragma once

#include <cstdint>

namespace Consolpp::Keyboard
{
    enum class KeyCode : std::uint8_t
    {
        Character,
        Enter,
        Tab,
        Backspace,
        Escape,
        Up,
        Down,
        Right,
        Left,
        Home,
        End,
        PageUp,
        PageDown,
        Insert,
        Delete,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
    };
} // namespace Consolpp::Keyboard
