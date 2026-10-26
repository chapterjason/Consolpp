#pragma once

#include "Consolpp/Completion/Request.hpp"

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Completion
{
    // Gets the name of the command and returns the values its complete member computes.
    using ComputeValues = std::function<std::vector<std::string>(std::string_view, const Request&)>;
} // namespace Consolpp::Completion
