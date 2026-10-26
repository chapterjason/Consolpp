#pragma once

namespace Consolpp::Text
{
    // Space, tab, line feed and carriage return, as XML 1.0 defines whitespace.
    constexpr bool isWhitespace(const char character)
    {
        return character == ' ' || character == '\t' || character == '\n' || character == '\r';
    }
} // namespace Consolpp::Text
