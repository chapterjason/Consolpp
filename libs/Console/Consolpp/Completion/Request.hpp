#pragma once

#include "Consolpp/Completion/ValueTarget.hpp"

#include <string_view>

namespace Consolpp::Completion
{
    // The value a command computes suggestions for.
    struct Request
    {
        public:
            ValueTarget target;
            // The name of the option or argument.
            std::string_view name;
            // What is typed of the value so far.
            std::string_view value;
    };
} // namespace Consolpp::Completion
