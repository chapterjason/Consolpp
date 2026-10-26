#pragma once

#include "Consolpp/Terminal/ColorMode.hpp"

namespace Consolpp::Terminal
{
    struct Capabilities
    {
        public:
            ColorMode colorMode{ColorMode::Colors16};
            bool hyperlinks{true};

            constexpr bool operator==(const Capabilities& other) const = default;
    };
} // namespace Consolpp::Terminal
