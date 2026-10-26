#pragma once

#include "Consolpp/Prompt/Selection.hpp"

#include <cstddef>
#include <span>
#include <string_view>

namespace Consolpp::Prompt
{
    // The texts and spans must outlive the field.
    struct ChoiceFieldOptions
    {
        public:
            // Markup.
            std::string_view label;
            std::span<const std::string_view> choices;
            Selection selection{Selection::Single};
            // Selected and, the first of them, highlighted at the start.
            std::span<const std::size_t> defaultIndexes{};
    };
} // namespace Consolpp::Prompt
