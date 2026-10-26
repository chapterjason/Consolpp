#pragma once

#include <cstdint>

namespace Consolpp::BuiltIn
{
    enum class Format : std::uint8_t
    {
        Text,
        Json,
        Markdown,
    };
} // namespace Consolpp::BuiltIn
