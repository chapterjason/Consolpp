#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace Consolpp::Event
{
    class Handler
    {
        private:
            static constexpr std::size_t capacity = 64;

            alignas(std::max_align_t) std::array<std::byte, capacity> storage{};
            void (*invoke)(void*);
            void (*destroy)(void*) noexcept;

            template <typename Stored>
            static void invokeStored(void* const target)
            {
                (*std::launder(static_cast<Stored*>(target)))();
            }

            template <typename Stored>
            static void destroyStored(void* const target) noexcept
            {
                std::destroy_at(std::launder(static_cast<Stored*>(target)));
            }

        public:
            template <typename Callable>
                requires std::invocable<std::decay_t<Callable>&>
                             && (!std::same_as<std::remove_cvref_t<Callable>, Handler>)
                             && (sizeof(std::decay_t<Callable>) <= Handler::capacity)
                             && (alignof(std::decay_t<Callable>) <= alignof(std::max_align_t))
            explicit Handler(Callable&& callable)
                : invoke{&Handler::invokeStored<std::decay_t<Callable>>},
                  destroy{&Handler::destroyStored<std::decay_t<Callable>>}
            {
                using Stored = std::decay_t<Callable>;

                ::new (this->storage.data()) Stored(std::forward<Callable>(callable));
            }

            Handler(const Handler&) = delete;
            Handler(Handler&&) = delete;
            Handler& operator=(const Handler&) = delete;
            Handler& operator=(Handler&&) = delete;

            ~Handler()
            {
                this->destroy(this->storage.data());
            }

            void operator()()
            {
                this->invoke(this->storage.data());
            }
    };
} // namespace Consolpp::Event
