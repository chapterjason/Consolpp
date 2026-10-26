#pragma once

namespace Consolpp::Keyboard
{
    struct Modifiers
    {
        public:
            bool shift{false};
            bool alt{false};
            bool control{false};

            constexpr bool operator==(const Modifiers& other) const = default;
    };
} // namespace Consolpp::Keyboard
