#pragma once

#include "Consolpp/Markup/Style.hpp"

#include <string_view>

namespace Consolpp::Markup
{
    struct ElementStyle
    {
        public:
            Style style{};
            // Still escaped as in the markup; empty without a link.
            std::string_view href{};

            constexpr bool operator==(const ElementStyle& other) const = default;
    };
} // namespace Consolpp::Markup
