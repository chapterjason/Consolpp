#pragma once

#include "Consolpp/Markup/Style.hpp"

#include <string_view>

namespace Consolpp::Markup
{
    // A named tag of the markup and the style its content gets.
    struct TagDefinition
    {
        public:
            std::string_view name;
            Style style;

            constexpr bool operator==(const TagDefinition& other) const = default;
    };
} // namespace Consolpp::Markup
