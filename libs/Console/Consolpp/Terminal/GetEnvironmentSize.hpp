#pragma once

#include <cstddef>

namespace Consolpp::Terminal
{
    // The positive number in the environment variable, else the fallback.
    std::size_t getEnvironmentSize(const char* name, std::size_t fallback);
} // namespace Consolpp::Terminal
