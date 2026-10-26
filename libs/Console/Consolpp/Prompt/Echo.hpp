#pragma once

#include <cstdint>

namespace Consolpp::Prompt
{
    // How the typed text is shown.
    enum class Echo : std::uint8_t
    {
        Visible,
        // A "*" for every grapheme, e.g. for passwords.
        Masked,
        Hidden,
    };
} // namespace Consolpp::Prompt
