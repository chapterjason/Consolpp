#include "Memory/CountingMemoryManager.hpp"

#include <benchmark/benchmark_api.h>
#include <benchmark/managers.h>

int main(int argc, char** argv)
{
    Consolpp::Tests::Benchmark::Memory::CountingMemoryManager memoryManager;

    benchmark::RegisterMemoryManager(&memoryManager);
    benchmark::Initialize(&argc, argv);

    if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    benchmark::RegisterMemoryManager(nullptr);

    return 0;
}
