#pragma once

#include "Consolpp/ValueCompletion.hpp"

#include <optional>
#include <span>
#include <string_view>

namespace Consolpp
{
    struct Argument
    {
        public:
            std::string_view name;
            bool required{true};
            bool variadic{false};
            std::string_view description{};
            std::optional<std::string_view> defaultValue{};
            std::span<const std::string_view> defaultValues{};
            // Either values the shell suggests or a completion mode, not both.
            std::span<const std::string_view> suggestedValues{};
            ValueCompletion completion{ValueCompletion::None};
    };
} // namespace Consolpp
