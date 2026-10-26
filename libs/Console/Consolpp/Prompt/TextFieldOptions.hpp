#pragma once

#include "Consolpp/Prompt/Autocompletion.hpp"
#include "Consolpp/Prompt/Echo.hpp"

#include <optional>
#include <string_view>

namespace Consolpp::Prompt
{
    // The texts must outlive the field.
    struct TextFieldOptions
    {
        public:
            // Markup.
            std::string_view label;
            // Taken when the answer is empty.
            std::optional<std::string_view> defaultValue{};
            Echo echo{Echo::Visible};
            std::optional<Autocompletion> autocompletion{};
    };
} // namespace Consolpp::Prompt
