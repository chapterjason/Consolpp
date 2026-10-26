#include "GetAllocationCounter.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <limits>
#include <new>

namespace
{
    void* allocateBlock(const std::size_t bytes, const std::size_t alignment)
    {
        if (alignment == alignof(std::max_align_t)) {
            return std::malloc(bytes); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
        }

        // aligned_alloc requires the size to be a multiple of the alignment.
        const std::size_t rounded = (bytes + alignment - 1) / alignment * alignment;

        return std::aligned_alloc(alignment, rounded); // NOLINT(cppcoreguidelines-owning-memory)
    }

    // Each block starts with a header holding the requested size, so deallocation can count the bytes.
    void* allocate(const std::size_t size, const std::size_t alignment)
    {
        if (size > std::numeric_limits<std::size_t>::max() - (2 * alignment)) {
            throw std::bad_alloc{};
        }

        void* block = allocateBlock(alignment + size, alignment);

        while (block == nullptr) {
            const std::new_handler handler = std::get_new_handler();

            if (handler == nullptr) {
                throw std::bad_alloc{};
            }

            handler();

            block = allocateBlock(alignment + size, alignment);
        }

        std::memcpy(block, &size, sizeof(size));

        Consolpp::Tests::Benchmark::Memory::getAllocationCounter().recordAllocation(size);

        return std::next(static_cast<std::byte*>(block), static_cast<std::ptrdiff_t>(alignment));
    }

    void deallocate(void* const pointer, const std::size_t alignment) noexcept
    {
        if (pointer == nullptr) {
            return;
        }

        void* const block = std::prev(static_cast<std::byte*>(pointer), static_cast<std::ptrdiff_t>(alignment));
        std::size_t size = 0;

        std::memcpy(&size, block, sizeof(size));

        Consolpp::Tests::Benchmark::Memory::getAllocationCounter().recordDeallocation(size);

        std::free(block); // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory)
    }

    std::size_t getAlignment(const std::align_val_t alignment)
    {
        return std::max(alignof(std::max_align_t), static_cast<std::size_t>(alignment));
    }
} // namespace

// The array and nothrow forms default to calling these, see [new.delete].
void* operator new(const std::size_t size)
{
    return allocate(size, alignof(std::max_align_t));
}

void* operator new(const std::size_t size, const std::align_val_t alignment)
{
    return allocate(size, getAlignment(alignment));
}

void operator delete(void* const pointer) noexcept
{
    deallocate(pointer, alignof(std::max_align_t));
}

void operator delete(void* const pointer, [[maybe_unused]] const std::size_t size) noexcept
{
    deallocate(pointer, alignof(std::max_align_t));
}

void operator delete(void* const pointer, const std::align_val_t alignment) noexcept
{
    deallocate(pointer, getAlignment(alignment));
}

void operator delete(void* const pointer,
                     [[maybe_unused]] const std::size_t size,
                     const std::align_val_t alignment) noexcept
{
    deallocate(pointer, getAlignment(alignment));
}
