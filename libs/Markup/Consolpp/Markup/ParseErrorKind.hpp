#pragma once

#include <cstdint>

namespace Consolpp::Markup
{
    enum class ParseErrorKind : std::uint8_t
    {
        MalformedTag,
        DuplicateAttribute,
        UnexpectedClosingTag,
        MismatchedClosingTag,
        UnclosedElement,
    };
} // namespace Consolpp::Markup
