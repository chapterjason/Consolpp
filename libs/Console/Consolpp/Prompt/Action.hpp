#pragma once

#include <cstdint>

namespace Consolpp::Prompt
{
    enum class Action : std::uint8_t
    {
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        MoveToStart,
        MoveToEnd,
        EraseBackward,
        EraseForward,
        Complete,
        Toggle,
        Confirm,
        Deny,
        Submit,
        Cancel,
        PreviousField,
    };
} // namespace Consolpp::Prompt
