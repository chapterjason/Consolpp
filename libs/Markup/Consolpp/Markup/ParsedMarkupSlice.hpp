#pragma once

#include <cstddef>

namespace Consolpp::Markup
{
    template <typename Context>
    class ParsedMarkup;

    // A part of parsed markup; it must not outlive the markup.
    template <typename Context>
    struct ParsedMarkupSlice
    {
        public:
            const ParsedMarkup<Context>* markup;
            std::size_t start;
            std::size_t end;
    };
} // namespace Consolpp::Markup
