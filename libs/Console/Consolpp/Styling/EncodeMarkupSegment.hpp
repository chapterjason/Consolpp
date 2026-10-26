#pragma once

#include "Consolpp/Markup/Decode.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Styling/EncodeLinkEnd.hpp"
#include "Consolpp/Styling/EncodeLinkStart.hpp"
#include "Consolpp/Styling/EncodeStyleEnd.hpp"
#include "Consolpp/Styling/EncodeStyleStart.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Styling
{
    // The text and href are still escaped as in the markup.
    template <std::output_iterator<char> Output>
    constexpr Output encodeMarkupSegment(const std::string_view escapedText,
                                         const Markup::Style& style,
                                         const std::string_view escapedHref,
                                         const Terminal::Capabilities& capabilities,
                                         Output output)
    {
        const bool linked = !escapedHref.empty() && capabilities.hyperlinks;

        output = Styling::encodeStyleStart(style, capabilities, output);

        if (linked) {
            std::string href;

            Markup::decode(escapedHref, std::back_inserter(href));
            output = Styling::encodeLinkStart(href, output);
        }

        output = Markup::decode(escapedText, output);

        if (linked) {
            output = Styling::encodeLinkEnd(output);
        }

        return Styling::encodeStyleEnd(style, output);
    }
} // namespace Consolpp::Styling
