#pragma once

#include <cstddef>
#include <cstdint>

namespace Consolpp::Markup
{
    struct Entity
    {
        public:
            std::uint32_t codePoint;
            std::size_t length;

            constexpr bool operator==(const Entity& other) const = default;
    };
} // namespace Consolpp::Markup
