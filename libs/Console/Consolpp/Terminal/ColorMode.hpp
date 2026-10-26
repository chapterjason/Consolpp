#pragma once

#include <cstdint>

namespace Consolpp::Terminal
{
    enum class ColorMode : std::uint8_t
    {
        Colors16,
        Colors256,
        TrueColor,
    };
} // namespace Consolpp::Terminal
