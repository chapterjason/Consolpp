#pragma once

#include <cstdint>

namespace Consolpp::Formatting
{
    enum class SynopsisForm : std::uint8_t
    {
        // "[options]" in place of the options.
        Short,
        // Every option on its own.
        Long,
    };
} // namespace Consolpp::Formatting
