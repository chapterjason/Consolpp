#pragma once

#include <cstdint>

namespace Consolpp::Prompt
{
    enum class Selection : std::uint8_t
    {
        Single,
        Multiple,
    };
} // namespace Consolpp::Prompt
