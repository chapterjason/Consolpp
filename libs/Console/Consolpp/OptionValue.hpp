#pragma once

#include <cstdint>

namespace Consolpp
{
    enum class OptionValue : std::uint8_t
    {
        None,
        Required,
        Optional,
    };
} // namespace Consolpp
