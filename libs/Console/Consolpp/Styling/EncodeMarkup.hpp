#pragma once

#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/Parser.hpp"
#include "Consolpp/Markup/ResolveElement.hpp"
#include "Consolpp/Styling/EncodeMarkupSegment.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <iterator>
#include <string_view>

namespace Consolpp::Styling
{
    // Checks the whole markup before writing, so invalid markup writes nothing.
    template <std::output_iterator<char> Output>
    Output encodeMarkup(const std::string_view markup, const Terminal::Capabilities& capabilities, Output output)
    {
        Markup::Parser{markup}.parse(Markup::ElementStyle{},
                                     &Markup::resolveElement,
                                     [](const std::string_view /*text*/, const Markup::ElementStyle& /*element*/) {
                                     });
        Markup::Parser{markup}.parse(
            Markup::ElementStyle{},
            &Markup::resolveElement,
            [&output, &capabilities](const std::string_view text, const Markup::ElementStyle& element) {
                output = Styling::encodeMarkupSegment(text, element.style, element.href, capabilities, output);
            });

        return output;
    }
} // namespace Consolpp::Styling
