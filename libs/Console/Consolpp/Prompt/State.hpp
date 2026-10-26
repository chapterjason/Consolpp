#pragma once

#include <cstdint>

namespace Consolpp::Prompt
{
    enum class State : std::uint8_t
    {
        Active,
        Submitted,
        Cancelled,
    };
} // namespace Consolpp::Prompt
