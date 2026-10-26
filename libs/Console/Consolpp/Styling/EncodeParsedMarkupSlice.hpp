#pragma once

#include "Consolpp/Markup/Decode.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/ParsedMarkupSlice.hpp"
#include "Consolpp/Markup/Run.hpp"
#include "Consolpp/Styling/EncodeLinkEnd.hpp"
#include "Consolpp/Styling/EncodeLinkStart.hpp"
#include "Consolpp/Styling/EncodeStyleEnd.hpp"
#include "Consolpp/Styling/EncodeStyleStart.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <span>
#include <string>
#include <string_view>

namespace Consolpp::Styling
{
    // Every style and link is closed inside the slice, so nothing reaches the text around it.
    template <std::output_iterator<char> Output>
    Output encodeParsedMarkupSlice(const Markup::ParsedMarkupSlice<Markup::ElementStyle>& slice,
                                   const Terminal::Capabilities& capabilities,
                                   Output output)
    {
        const Markup::ParsedMarkup<Markup::ElementStyle>& markup = *slice.markup;
        const std::span<const Markup::Run<Markup::ElementStyle>> runs = markup.getRuns();
        const auto first = std::ranges::partition_point(runs, [&slice](const Markup::Run<Markup::ElementStyle>& run) {
            return run.start + run.length <= slice.start;
        });

        for (auto run = first; run != runs.end() && run->start < slice.end; ++run) {
            const std::size_t start = std::max(run->start, slice.start);
            const std::size_t end = std::min(run->start + run->length, slice.end);
            const bool linked = !run->context.href.empty() && capabilities.hyperlinks;

            output = Styling::encodeStyleStart(run->context.style, capabilities, output);

            if (linked) {
                std::string href;

                Markup::decode(run->context.href, std::back_inserter(href));
                output = Styling::encodeLinkStart(href, output);
            }

            output = std::ranges::copy(markup.getText().substr(start, end - start), output).out;

            if (linked) {
                output = Styling::encodeLinkEnd(output);
            }

            output = Styling::encodeStyleEnd(run->context.style, output);
        }

        return output;
    }
} // namespace Consolpp::Styling
