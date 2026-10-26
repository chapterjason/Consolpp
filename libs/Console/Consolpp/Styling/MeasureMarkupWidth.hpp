#pragma once

#include "Consolpp/Markup/Decode.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/Parser.hpp"
#include "Consolpp/Markup/ResolveElement.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Styling
{
    // The display width of the text before the offset into the markup; the offset must not lie inside a tag or an
    // entity. Throws like parseMarkup() for invalid markup.
    inline std::size_t measureMarkupWidth(const std::string_view markup, const std::size_t end)
    {
        std::size_t width = 0;
        std::string decoded;

        Markup::Parser{markup}.parse(
            Markup::ElementStyle{},
            &Markup::resolveElement,
            [markup, end, &width, &decoded](const std::string_view escapedText, const Markup::ElementStyle& /*style*/) {
                // The parser passes views into the markup, and only non-empty ones.
                const auto start = static_cast<std::size_t>(std::distance(&markup.front(), &escapedText.front()));

                if (start >= end) {
                    return;
                }

                decoded.clear();
                Markup::decode(escapedText.substr(0, end - start), std::back_inserter(decoded));
                width += Text::measureDisplayWidth(decoded);
            });

        return width;
    }
} // namespace Consolpp::Styling
