#pragma once

#include <cstddef>
#include <string>

namespace Consolpp::Prompt
{
    // The {input} of the input line, with byte offsets into it.
    struct InputLineText
    {
        public:
            std::string text;
            std::size_t cursor{0};
    };
} // namespace Consolpp::Prompt
