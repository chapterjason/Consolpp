#pragma once

#include <cstdint>

namespace Consolpp::Markup
{
    enum class DecodeErrorKind : std::uint8_t
    {
        UnknownEntity,
        UnterminatedEntity,
        InvalidCharacterReference,
    };
} // namespace Consolpp::Markup
