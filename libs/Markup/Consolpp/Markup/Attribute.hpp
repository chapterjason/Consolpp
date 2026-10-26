#pragma once

#include <cstddef>
#include <string_view>

namespace Consolpp::Markup
{
    struct Attribute
    {
        public:
            std::string_view name;
            std::size_t namePosition;
            // Still escaped as in the markup.
            std::string_view value;
            std::size_t valuePosition;

            constexpr bool operator==(const Attribute& other) const = default;
    };
} // namespace Consolpp::Markup
