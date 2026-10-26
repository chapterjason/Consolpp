#pragma once

#include <cstdint>

namespace Consolpp::Tools::Screen
{
    enum class EntryKind : std::uint8_t
    {
        Output,
        Resize,
    };
} // namespace Consolpp::Tools::Screen
