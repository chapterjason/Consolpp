#pragma once

#include "Consolpp/Command.hpp"

#include <array>
#include <cstddef>
#include <string_view>

namespace Consolpp
{
    template <bool IncludeHidden, Command... Commands>
    consteval auto getCommandNames()
    {
        constexpr std::size_t count = (0U
                                       + ...
                                       + (IncludeHidden || !Commands::definition.hidden
                                              ? 1 + Commands::definition.aliases.size()
                                              : 0));
        std::array<std::string_view, count> names{};
        std::size_t position = 0;
        [[maybe_unused]]
        const auto collectName = [&names, &position](const auto& definition) {
            if (IncludeHidden || !definition.hidden) {
                names.at(position) = definition.name;
                ++position;
            }
        };
        [[maybe_unused]]
        const auto collectAliases = [&names, &position](const auto& definition) {
            if (!IncludeHidden && definition.hidden) {
                return;
            }

            for (const std::string_view alias : definition.aliases) {
                names.at(position) = alias;
                ++position;
            }
        };

        (collectName(Commands::definition), ...);
        (collectAliases(Commands::definition), ...);

        return names;
    }
} // namespace Consolpp
