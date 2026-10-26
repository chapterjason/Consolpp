#pragma once

#include <string_view>

namespace Consolpp
{
    // The program the commands belong to, as shown in help such as man pages.
    struct Program
    {
        public:
            std::string_view name;
            std::string_view version{};
    };
} // namespace Consolpp
