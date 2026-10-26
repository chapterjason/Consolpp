#pragma once

#include <cstddef>

namespace Consolpp::Markup
{
    // A part of the parsed text with the context of its innermost element.
    template <typename Context>
    struct Run
    {
        public:
            std::size_t start{0};
            std::size_t length{0};
            Context context;
    };
} // namespace Consolpp::Markup
