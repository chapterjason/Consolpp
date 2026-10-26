#pragma once

#include "Consolpp/OptionValue.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <optional>
#include <span>
#include <string_view>

namespace Consolpp
{
    struct Option
    {
        public:
            std::string_view name;
            std::optional<char> shortcut{};
            OptionValue value{OptionValue::None};
            bool repeatable{false};
            std::string_view description{};
            std::optional<std::string_view> defaultValue{};
            std::span<const std::string_view> defaultValues{};
            std::optional<bool> defaultEnabled{};
            // Either values the shell suggests or a completion mode, not both.
            std::span<const std::string_view> suggestedValues{};
            ValueCompletion completion{ValueCompletion::None};
    };
} // namespace Consolpp
