#pragma once

#include <cstdint>

namespace Consolpp::Text
{
    // Both ends are included.
    struct CodePointRange
    {
        public:
            std::uint32_t first;
            std::uint32_t last;
    };
} // namespace Consolpp::Text
