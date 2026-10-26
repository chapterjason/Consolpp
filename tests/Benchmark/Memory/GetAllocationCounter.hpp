#pragma once

#include "AllocationCounter.hpp"

namespace Consolpp::Tests::Benchmark::Memory
{
    inline AllocationCounter& getAllocationCounter()
    {
        static AllocationCounter counter;

        return counter;
    }
} // namespace Consolpp::Tests::Benchmark::Memory
