#pragma once

#include "Consolpp/DefinitionView.hpp"

#include <string_view>

namespace Consolpp::Formatting
{
    struct ListEntry
    {
        public:
            std::string_view name;
            const DefinitionView* command;
    };
} // namespace Consolpp::Formatting
