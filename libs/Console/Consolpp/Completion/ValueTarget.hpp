#pragma once

#include <cstdint>

namespace Consolpp::Completion
{
    enum class ValueTarget : std::uint8_t
    {
        Option,
        Argument,
    };
} // namespace Consolpp::Completion
