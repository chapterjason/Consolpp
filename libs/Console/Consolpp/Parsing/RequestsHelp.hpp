#pragma once

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <span>

namespace Consolpp::Parsing
{
    // Whether "--help" or the shortcut "h" is given before "--", read like the parser reads the options, so values
    // such as "-ohello" or "--output -h" do not count.
    bool requestsHelp(std::span<const Option> options, TokenRange tokens);
} // namespace Consolpp::Parsing
