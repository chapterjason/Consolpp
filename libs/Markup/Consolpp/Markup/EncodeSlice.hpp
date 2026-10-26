#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/ColorAttributes.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Markup/EncodeColor.hpp"
#include "Consolpp/Markup/FlagAttributes.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/ParsedMarkupSlice.hpp"
#include "Consolpp/Markup/Run.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>
#include <span>
#include <string_view>

namespace Consolpp::Markup
{
    // Styled text becomes style and link elements instead of the tags it came from; it looks the same, whatever the
    // registered tags are.
    template <std::output_iterator<char> Output>
    Output encodeSlice(const ParsedMarkupSlice<ElementStyle>& slice, Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const ParsedMarkup<ElementStyle>& markup = *slice.markup;
        const std::span<const Run<ElementStyle>> runs = markup.getRuns();
        const auto first = std::ranges::partition_point(runs, [&slice](const Run<ElementStyle>& run) {
            return run.start + run.length <= slice.start;
        });

        for (auto run = first; run != runs.end() && run->start < slice.end; ++run) {
            const std::size_t start = std::max(run->start, slice.start);
            const std::size_t end = std::min(run->start + run->length, slice.end);
            const std::string_view text = markup.getText().substr(start, end - start);
            const ElementStyle& element = run->context;
            const std::string_view name = element.href.empty() ? "style" : "link";

            if (element == ElementStyle{}) {
                output = Markup::encode(text, output);

                continue;
            }

            copy("<");
            copy(name);

            if (!element.href.empty()) {
                copy(" href=\"");
                copy(element.href);
                copy("\"");
            }

            for (const auto& [attribute, member] : Markup::colorAttributes) {
                const std::optional<Color>& color = element.style.*member;

                if (color.has_value()) {
                    copy(" ");
                    copy(attribute);
                    copy("=\"");
                    output = Markup::encodeColor(*color, output);
                    copy("\"");
                }
            }

            for (const auto& [attribute, member] : Markup::flagAttributes) {
                if (element.style.*member) {
                    copy(" ");
                    copy(attribute);
                    copy("=\"true\"");
                }
            }

            copy(">");
            output = Markup::encode(text, output);
            copy("</");
            copy(name);
            copy(">");
        }

        return output;
    }
} // namespace Consolpp::Markup
