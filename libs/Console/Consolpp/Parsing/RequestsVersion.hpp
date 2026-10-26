#pragma once

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <span>

namespace Consolpp::Parsing
{
    // Whether "--version" is given before "--", read like the parser reads the options, so a value such as
    // "--output --version" does not count.
    bool requestsVersion(std::span<const Option> options, TokenRange tokens);
} // namespace Consolpp::Parsing
