#pragma once

#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/ResolveElement.hpp"

#include <string_view>

namespace Consolpp::Styling
{
    // Throws ParseError, ElementError or DecodeError for invalid markup.
    inline Markup::ParsedMarkup<Markup::ElementStyle> parseMarkup(const std::string_view markup)
    {
        return Markup::ParsedMarkup<Markup::ElementStyle>::fromMarkup(markup,
                                                                      Markup::ElementStyle{},
                                                                      &Markup::resolveElement);
    }
} // namespace Consolpp::Styling
