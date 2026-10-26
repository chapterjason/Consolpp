#pragma once

#include <cstdint>

namespace Consolpp::Terminal
{
    enum class DecoderMode : std::uint8_t
    {
        Keys,
        Paste,
    };
} // namespace Consolpp::Terminal
