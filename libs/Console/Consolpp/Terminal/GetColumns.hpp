#pragma once

#include <cstddef>

namespace Consolpp::Terminal
{
    // The columns of the terminal behind the descriptor, else COLUMNS from the environment, else 80.
    std::size_t getColumns(int descriptor);
} // namespace Consolpp::Terminal
