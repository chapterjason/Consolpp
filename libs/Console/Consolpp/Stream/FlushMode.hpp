#pragma once

#include <cstdint>

namespace Consolpp::Stream
{
    enum class FlushMode : std::uint8_t
    {
        Buffered,
        Line,
        Immediate,
    };
} // namespace Consolpp::Stream
