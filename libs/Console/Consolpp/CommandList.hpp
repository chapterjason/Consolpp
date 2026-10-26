#pragma once

#include "Consolpp/DefinitionView.hpp"

#include <span>
#include <string_view>

namespace Consolpp
{
    // Every command a CommandManager runs, for commands that describe the others.
    struct CommandList
    {
        public:
            std::span<const DefinitionView> definitions;
            // The visible names and aliases in natural order, the candidates of suggestions.
            std::span<const std::string_view> names;
            bool (*isSuggestion)(std::string_view, std::string_view);
    };
} // namespace Consolpp
