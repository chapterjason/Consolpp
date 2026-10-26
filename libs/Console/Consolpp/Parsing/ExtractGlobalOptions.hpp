#pragma once

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/GlobalOptionValues.hpp"

#include <span>
#include <vector>

namespace Consolpp::Parsing
{
    // Takes -q, --quiet, --silent, -v, -vv, -vvv, --verbose, -n and --no-interaction out of the arguments before "--";
    // the others go to remaining. Values of the options, which are read like the parser reads them, are left alone.
    GlobalOptionValues extractGlobalOptions(std::span<const Option> options,
                                            std::span<const char* const> arguments,
                                            std::vector<const char*>& remaining);
} // namespace Consolpp::Parsing
