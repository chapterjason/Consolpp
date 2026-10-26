#pragma once

#include <benchmark/managers.h>

namespace Consolpp::Tests::Benchmark::Memory
{
    class CountingMemoryManager : public benchmark::MemoryManager
    {
        public:
            void Start() override;

            void Stop(benchmark::MemoryManager::Result& result) override;
    };
} // namespace Consolpp::Tests::Benchmark::Memory
