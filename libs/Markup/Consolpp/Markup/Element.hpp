#pragma once

#include "Consolpp/Markup/AttributeRange.hpp"

#include <cstddef>
#include <string_view>

namespace Consolpp::Markup
{
    struct Element
    {
        public:
            std::string_view name;
            // Where the "<" is.
            std::size_t position;
            // Checked by the parser and still escaped as in the markup.
            std::string_view attributes;
            std::size_t attributesPosition;
            bool selfClosing;

            [[nodiscard]]
            constexpr AttributeRange getAttributes() const
            {
                return AttributeRange{this->attributes, this->attributesPosition};
            }
    };
} // namespace Consolpp::Markup
