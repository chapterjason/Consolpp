#pragma once

#include <cstdint>

namespace Consolpp::Event
{
    enum class SourceKind : std::uint8_t
    {
        Readable,
        Writable,
        Timer,
        Timeout,
        Immediate,
        Signal,
    };
} // namespace Consolpp::Event
