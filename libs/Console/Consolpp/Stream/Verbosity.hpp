#pragma once

#include <cstdint>

namespace Consolpp::Stream
{
    // Ordered, so that a message is written while its level is at most the stream's.
    enum class Verbosity : std::uint8_t
    {
        Silent,
        Quiet,
        Normal,
        Verbose,
        VeryVerbose,
        Debug,
    };
} // namespace Consolpp::Stream
