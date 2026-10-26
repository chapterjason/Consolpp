#pragma once

#include <cstddef>

namespace Consolpp::Terminal
{
    // The rows of the terminal behind the descriptor, else LINES from the environment, else 24.
    std::size_t getRows(int descriptor);
} // namespace Consolpp::Terminal
