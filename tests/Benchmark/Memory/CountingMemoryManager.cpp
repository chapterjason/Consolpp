#include "CountingMemoryManager.hpp"

#include "AllocationCounter.hpp"
#include "GetAllocationCounter.hpp"

#include <benchmark/managers.h>

namespace Consolpp::Tests::Benchmark::Memory
{
    void CountingMemoryManager::Start()
    {
        getAllocationCounter().start();
    }

    void CountingMemoryManager::Stop(benchmark::MemoryManager::Result& result)
    {
        AllocationCounter& counter = getAllocationCounter();

        counter.stop();

        result.num_allocs = counter.getAllocations();
        result.max_bytes_used = counter.getPeakBytes();
        result.total_allocated_bytes = counter.getAllocatedBytes();
        result.net_heap_growth = counter.getCurrentBytes();
    }
} // namespace Consolpp::Tests::Benchmark::Memory
