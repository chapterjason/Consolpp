#pragma once

#include <cstdint>

namespace Consolpp::Prompt
{
    // How pasted control characters enter a text field; it shows the ones kept as U+FFFD.
    enum class ControlCharacters : std::uint8_t
    {
        Drop,
        // Line breaks and tabs by a space, the others by U+FFFD.
        Replace,
        Keep,
    };
} // namespace Consolpp::Prompt
