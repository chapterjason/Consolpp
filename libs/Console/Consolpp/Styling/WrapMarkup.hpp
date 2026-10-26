#pragma once

#include "Consolpp/Markup/Decode.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/EncodeSlice.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/Parser.hpp"
#include "Consolpp/Markup/ResolveElement.hpp"
#include "Consolpp/Styling/MarkupLine.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"
#include "Consolpp/Text/Splitter.hpp"
#include "Consolpp/Text/WrapIterator.hpp"
#include "Consolpp/Text/WrappedLine.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Styling
{
    // Appends the lines of the markup, wrapped at the display width, to the text and their places to the lines.
    // Markup that fits on one line stays as it is; wrapped lines are written back with style and link elements, so
    // every line is markup of its own. Throws like parseMarkup() for invalid markup.
    inline void wrapMarkup(const std::string_view markup,
                           const std::size_t width,
                           std::string& text,
                           std::vector<MarkupLine>& lines)
    {
        std::size_t widest = 0;
        std::size_t current = 0;
        bool broken = false;
        std::string decoded;

        Markup::Parser{markup}.parse(Markup::ElementStyle{},
                                     &Markup::resolveElement,
                                     [&widest, &current, &broken, &decoded](const std::string_view escapedText,
                                                                            const Markup::ElementStyle& /*style*/) {
                                         std::string_view visible = escapedText;

                                         if (escapedText.find('&') != std::string_view::npos) {
                                             decoded.clear();
                                             Markup::decode(escapedText, std::back_inserter(decoded));
                                             visible = decoded;
                                         }

                                         for (Text::Splitter parts{visible, '\n'};;) {
                                             current += Text::measureDisplayWidth(parts.takeNext());

                                             if (!parts.hasNext()) {
                                                 break;
                                             }

                                             broken = true;
                                             widest = std::max(widest, current);
                                             current = 0;
                                         }
                                     });

        widest = std::max(widest, current);

        if (!broken && widest <= width) {
            lines.push_back(MarkupLine{.start = text.size(), .length = markup.size(), .width = widest});
            text.append(markup);

            return;
        }

        const Markup::ParsedMarkup<Markup::ElementStyle> parsed = Styling::parseMarkup(markup);

        for (Text::WrapIterator wrapped{parsed.getText(), std::max<std::size_t>(width, 1)};
             wrapped != std::default_sentinel;
             ++wrapped) {
            const Text::WrappedLine& line = *wrapped;
            const std::size_t start = text.size();

            Markup::encodeSlice(parsed.getSlice(line.start, line.end), std::back_inserter(text));
            lines.push_back(MarkupLine{.start = start, .length = text.size() - start, .width = line.width});
        }
    }
} // namespace Consolpp::Styling
