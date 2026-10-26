#pragma once

#include <cstdint>

namespace Consolpp::Parsing
{
    struct VariadicState
    {
        public:
            std::uint32_t firstPosition{0};
            bool afterSeparator{false};
    };
} // namespace Consolpp::Parsing
