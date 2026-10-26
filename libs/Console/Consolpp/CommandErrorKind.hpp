#pragma once

#include <cstdint>

namespace Consolpp
{
    enum class CommandErrorKind : std::uint8_t
    {
        UnknownCommand,
        UnknownNamespace,
    };
} // namespace Consolpp
