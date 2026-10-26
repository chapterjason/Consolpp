#pragma once

#include <cstddef>
#include <span>

namespace Consolpp::Stream
{
    // Zero at the end of the input.
    std::size_t readDescriptor(int descriptor, std::span<char> target);
} // namespace Consolpp::Stream
