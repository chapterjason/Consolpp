#pragma once

#include "Consolpp/Text/Grapheme/BreakState.hpp"
#include "Consolpp/Text/Utf8/CodePoint.hpp"
#include "Consolpp/Text/Utf8/Decode.hpp"

#include <cstddef>
#include <string_view>

namespace Consolpp::Text::Grapheme
{
    // The text must be valid UTF-8.
    constexpr std::size_t findEnd(const std::string_view text, const std::size_t start)
    {
        const Utf8::CodePoint first = Text::Utf8::decode(text.substr(start));
        BreakState state{first.value};
        std::size_t end = start + first.length;

        while (end < text.size()) {
            const Utf8::CodePoint next = Text::Utf8::decode(text.substr(end));

            if (!state.joins(next.value)) {
                break;
            }

            state.append(next.value);
            end += next.length;
        }

        return end;
    }
} // namespace Consolpp::Text::Grapheme
