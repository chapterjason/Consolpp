#pragma once

#include <cstdint>

namespace Consolpp::Markup
{
    enum class ElementErrorKind : std::uint8_t
    {
        UnknownElement,
        UnknownAttribute,
        MissingAttribute,
        InvalidAttributeValue,
    };
} // namespace Consolpp::Markup
