#pragma once

#include <optional>
#include <string_view>

namespace Consolpp::Prompt
{
    // The label must outlive the field.
    struct ConfirmationFieldOptions
    {
        public:
            // Markup.
            std::string_view label;
            // Taken when nothing was typed; without one, an answer is needed.
            std::optional<bool> defaultValue{};
    };
} // namespace Consolpp::Prompt
