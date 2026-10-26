#pragma once

#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/Parser.hpp"

#include <string_view>
#include <variant>

namespace Consolpp::Markup
{
    // Checks the tags and entities; whether a tag is registered is only known at runtime.
    // Throws ParseError or DecodeError for invalid markup.
    constexpr void validateSyntax(const std::string_view markup)
    {
        Parser{markup}.parse(
            std::monostate{},
            [](const Element& /*element*/) {
                return std::monostate{};
            },
            [](const std::string_view /*text*/, const std::monostate /*context*/) {
            });
    }
} // namespace Consolpp::Markup
