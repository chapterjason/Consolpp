#pragma once

#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Prompt/Action.hpp"

namespace Consolpp::Prompt
{
    struct KeyBinding
    {
        public:
            Keyboard::Key key;
            Action action{Action::MoveLeft};
    };
} // namespace Consolpp::Prompt
