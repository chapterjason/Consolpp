#pragma once

#include "Consolpp/Markup/Decode.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/Parser.hpp"
#include "Consolpp/Markup/ResolveElement.hpp"

#include <iterator>
#include <string_view>

namespace Consolpp::Styling
{
    // Checks the whole markup before writing, so invalid markup writes nothing.
    template <std::output_iterator<char> Output>
    Output stripMarkup(const std::string_view markup, Output output)
    {
        Markup::Parser{markup}.parse(Markup::ElementStyle{},
                                     &Markup::resolveElement,
                                     [](const std::string_view /*text*/, const Markup::ElementStyle& /*element*/) {
                                     });
        Markup::Parser{markup}.parse(Markup::ElementStyle{},
                                     &Markup::resolveElement,
                                     [&output](const std::string_view text, const Markup::ElementStyle& /*element*/) {
                                         output = Markup::decode(text, output);
                                     });

        return output;
    }
} // namespace Consolpp::Styling
