#pragma once

#include "Consolpp/Option.hpp"

#include <array>

namespace Consolpp
{
    // Options of every command, handled before the command runs; a signature cannot declare their names or shortcuts.
    inline constexpr std::array<Option, 6> globalOptions{
        Option{.name = "help", .shortcut = 'h', .description = "Display help for the command"},
        Option{.name = "version", .description = "Display the program version"},
        Option{
            .name = "quiet",
            .shortcut = 'q',
            .description = "Only errors are displayed. All other output is suppressed",
        },
        Option{.name = "silent", .description = "Do not output any message"},
        Option{
            .name = "verbose",
            .shortcut = 'v',
            .description = "Increase the verbosity of messages: once for verbose output, twice for more, three times "
                           "for debug",
        },
        Option{.name = "no-interaction", .shortcut = 'n', .description = "Do not ask any interactive question"},
    };
} // namespace Consolpp
