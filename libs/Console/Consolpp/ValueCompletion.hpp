#pragma once

#include <cstdint>

namespace Consolpp
{
    // How the shell completes the value of an option or argument, unless it has suggested values.
    enum class ValueCompletion : std::uint8_t
    {
        None,
        // The values the complete member of the command returns.
        Computed,
        // Files, with directories offered only to descend into them.
        File,
        Directory,
        // Files and directories, both as the value.
        Path,
    };
} // namespace Consolpp
