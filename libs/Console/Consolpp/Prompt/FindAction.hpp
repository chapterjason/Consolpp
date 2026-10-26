#pragma once

#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/KeyBinding.hpp"

#include <algorithm>
#include <optional>
#include <span>

namespace Consolpp::Prompt
{
    // The first binding of the key, matched with its modifiers; nothing for an unbound key.
    constexpr std::optional<Action> findAction(const std::span<const KeyBinding> keyBindings, const Keyboard::Key& key)
    {
        const auto found = std::ranges::find(keyBindings, key, &KeyBinding::key);

        return found == keyBindings.end() ? std::nullopt : std::optional<Action>{found->action};
    }
} // namespace Consolpp::Prompt
