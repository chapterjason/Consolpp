#pragma once

#include "Consolpp/ValueCompletion.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Completion
{
    struct Result
    {
        public:
            // Whole words, each starting with what is typed of the word.
            std::vector<std::string> suggestions{};
            // File, Directory or Path when the shell completes the value from the file system instead.
            ValueCompletion paths{ValueCompletion::None};
            // What is typed of the word before the value, e.g. "--file=", which the shell keeps for the paths.
            std::string_view valuePrefix{};
    };
} // namespace Consolpp::Completion
