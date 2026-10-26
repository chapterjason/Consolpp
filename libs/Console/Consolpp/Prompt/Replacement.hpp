#pragma once

#include <string_view>

namespace Consolpp::Prompt
{
    struct Replacement
    {
        public:
            std::string_view name;
            std::string_view value;
    };
} // namespace Consolpp::Prompt
