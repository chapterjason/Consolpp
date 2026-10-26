#pragma once

#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Keyboard/Modifiers.hpp"

namespace Consolpp::Keyboard
{
    struct Key
    {
        public:
            KeyCode code{KeyCode::Character};
            // The code point of a Character key; zero for every other key.
            char32_t character{0};
            Modifiers modifiers{};

            constexpr bool operator==(const Key& other) const = default;
    };
} // namespace Consolpp::Keyboard
