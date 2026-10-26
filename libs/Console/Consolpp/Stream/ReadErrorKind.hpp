#pragma once

#include <cstdint>

namespace Consolpp::Stream
{
    enum class ReadErrorKind : std::uint8_t
    {
        EndOfInput,
        InvalidValue,
        OutOfRange,
    };
} // namespace Consolpp::Stream
