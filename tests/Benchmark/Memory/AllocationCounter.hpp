#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace Consolpp::Tests::Benchmark::Memory
{
    class AllocationCounter
    {
        private:
            std::atomic<bool> tracking{false};
            std::atomic<std::int64_t> allocations{0};
            std::atomic<std::int64_t> allocatedBytes{0};
            std::atomic<std::int64_t> currentBytes{0};
            std::atomic<std::int64_t> peakBytes{0};

        public:
            void start()
            {
                this->allocations.store(0, std::memory_order_relaxed);
                this->allocatedBytes.store(0, std::memory_order_relaxed);
                this->currentBytes.store(0, std::memory_order_relaxed);
                this->peakBytes.store(0, std::memory_order_relaxed);
                this->tracking.store(true, std::memory_order_relaxed);
            }

            void stop()
            {
                this->tracking.store(false, std::memory_order_relaxed);
            }

            void recordAllocation(const std::size_t size)
            {
                if (!this->tracking.load(std::memory_order_relaxed)) {
                    return;
                }

                const auto bytes = static_cast<std::int64_t>(size);
                const std::int64_t current = this->currentBytes.fetch_add(bytes, std::memory_order_relaxed) + bytes;
                std::int64_t peak = this->peakBytes.load(std::memory_order_relaxed);

                this->allocations.fetch_add(1, std::memory_order_relaxed);
                this->allocatedBytes.fetch_add(bytes, std::memory_order_relaxed);

                while (current > peak
                       && !this->peakBytes.compare_exchange_weak(peak, current, std::memory_order_relaxed)) {
                }
            }

            void recordDeallocation(const std::size_t size)
            {
                if (!this->tracking.load(std::memory_order_relaxed)) {
                    return;
                }

                this->currentBytes.fetch_sub(static_cast<std::int64_t>(size), std::memory_order_relaxed);
            }

            [[nodiscard]]
            std::int64_t getAllocations() const
            {
                return this->allocations.load(std::memory_order_relaxed);
            }

            [[nodiscard]]
            std::int64_t getAllocatedBytes() const
            {
                return this->allocatedBytes.load(std::memory_order_relaxed);
            }

            [[nodiscard]]
            std::int64_t getCurrentBytes() const
            {
                return this->currentBytes.load(std::memory_order_relaxed);
            }

            [[nodiscard]]
            std::int64_t getPeakBytes() const
            {
                return this->peakBytes.load(std::memory_order_relaxed);
            }
    };
} // namespace Consolpp::Tests::Benchmark::Memory
