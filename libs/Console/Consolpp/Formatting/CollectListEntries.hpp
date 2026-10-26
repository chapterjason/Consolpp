#pragma once

#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/ListEntry.hpp"

#include <span>
#include <string_view>
#include <vector>

namespace Consolpp::Formatting
{
    // Throws CommandError when a non-empty namespace holds no listed name.
    std::vector<ListEntry> collectListEntries(std::span<const DefinitionView> commands, std::string_view namespaceName);
} // namespace Consolpp::Formatting
