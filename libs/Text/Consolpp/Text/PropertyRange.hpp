#pragma once

#include <cstdint>

namespace Consolpp::Text
{
    // Both ends are included.
    template <typename Value>
    struct PropertyRange
    {
        public:
            std::uint32_t first;
            std::uint32_t last;
            Value value;
    };
} // namespace Consolpp::Text
