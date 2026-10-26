#pragma once

#include <cstdint>

namespace Consolpp
{
    enum class RunArgument : std::uint8_t
    {
        Parameters,
        Input,
        Output,
        ErrorOutput,
        Loop,
        Commands,
        Program,
    };
} // namespace Consolpp
