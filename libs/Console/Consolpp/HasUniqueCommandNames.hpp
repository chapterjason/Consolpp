#pragma once

#include "Consolpp/Command.hpp"
#include "Consolpp/GetCommandNames.hpp"

#include <algorithm>

namespace Consolpp
{
    template <Command... Commands>
    consteval bool hasUniqueCommandNames()
    {
        auto names = Consolpp::getCommandNames<true, Commands...>();

        std::ranges::sort(names);

        return std::ranges::adjacent_find(names) == names.end();
    }
} // namespace Consolpp
