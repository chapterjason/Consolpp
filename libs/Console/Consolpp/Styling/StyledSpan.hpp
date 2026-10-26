#pragma once

#include "Consolpp/Markup/Style.hpp"

#include <string_view>

namespace Consolpp::Styling
{
    // The text is written as it is, never parsed as markup.
    struct StyledSpan
    {
        public:
            Markup::Style style;
            std::string_view text;
    };
} // namespace Consolpp::Styling
