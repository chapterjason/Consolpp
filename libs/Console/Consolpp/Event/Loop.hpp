#pragma once

#include "Consolpp/Event/DescriptorRegistrations.hpp"
#include "Consolpp/Event/Handler.hpp"
#include "Consolpp/Event/Registration.hpp"
#include "Consolpp/Event/SourceKind.hpp"

#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

// POSIX sigset_t and sigaction are not declared through <csignal>.
#include <signal.h> // NOLINT(modernize-deprecated-headers)

namespace Consolpp::Event
{
    // Signals below this number can be registered; covers SIGRTMAX on Linux, macOS signals end at 31.
    inline constexpr std::size_t signalLimit = 65;

    // A handler may end its own registration as its last action; it must not touch its captures afterwards.
    // Not thread-safe: registering, ending registrations and running happen on one thread. On Linux a registered
    // signal is blocked in that thread; other threads must block it as well, or they still receive it.
    class Loop
    {
        private:
            int queue{-1};
            std::unordered_map<int, DescriptorRegistrations> descriptors;
            std::unordered_map<int, Registration*> timers;
            std::array<Registration*, signalLimit> heads{};
            std::vector<Registration*> immediates;
            // Taken from immediates for the current round; one an exception skipped stays here for the next.
            std::vector<Registration*> dueImmediates;
            int signals{-1};
#ifdef __linux__
            sigset_t signalMask{};
            std::array<bool, signalLimit> blockedBefore{};
#elif defined(__APPLE__)
            std::array<struct sigaction, signalLimit> previousActions{};
            int nextTimer{0};
#endif
            bool stopped{false};

            void attach(Registration& registration);

            void detach(Registration& registration) noexcept;

            void attachDescriptor(Registration& registration);

            void detachDescriptor(Registration& registration) noexcept;

            void attachTimer(Registration& registration);

            void detachTimer(Registration& registration) noexcept;

            void attachSignal(Registration& registration);

            void detachSignal(Registration& registration) noexcept;

            void attachImmediate(Registration& registration);

            void detachImmediate(Registration& registration) noexcept;

            [[nodiscard]]
            bool hasImmediates() const;

            void dispatchSignals();

            void dispatchDescriptor(int descriptor, bool readable, bool writable);

            void dispatchTimer(int timer);

            void dispatchSignal(int signal);

            void dispatchImmediates();

            // Without a deadline it waits until a source is ready.
            bool waitAndDispatch(std::optional<std::chrono::steady_clock::time_point> deadline);

            friend class Registration;

        public:
            Loop();

            Loop(const Loop&) = delete;
            Loop(Loop&&) = delete;
            Loop& operator=(const Loop&) = delete;
            Loop& operator=(Loop&&) = delete;

            ~Loop();

            template <typename Callable>
                requires std::constructible_from<Handler, Callable>
            [[nodiscard]]
            Registration registerReadable(const int descriptor, Callable&& callable)
            {
                return Registration{
                    *this,
                    SourceKind::Readable,
                    descriptor,
                    std::chrono::milliseconds{0},
                    std::forward<Callable>(callable),
                };
            }

            template <typename Callable>
                requires std::constructible_from<Handler, Callable>
            [[nodiscard]]
            Registration registerWritable(const int descriptor, Callable&& callable)
            {
                return Registration{
                    *this,
                    SourceKind::Writable,
                    descriptor,
                    std::chrono::milliseconds{0},
                    std::forward<Callable>(callable),
                };
            }

            template <typename Callable>
                requires std::constructible_from<Handler, Callable>
            [[nodiscard]]
            Registration registerTimer(const std::chrono::milliseconds interval, Callable&& callable)
            {
                return Registration{*this, SourceKind::Timer, 0, interval, std::forward<Callable>(callable)};
            }

            template <typename Callable>
                requires std::constructible_from<Handler, Callable>
            [[nodiscard]]
            Registration registerTimeout(const std::chrono::milliseconds delay, Callable&& callable)
            {
                return Registration{*this, SourceKind::Timeout, 0, delay, std::forward<Callable>(callable)};
            }

            // Calls the handler once in the next round of the loop, without waiting for another source.
            template <typename Callable>
                requires std::constructible_from<Handler, Callable>
            [[nodiscard]]
            Registration registerImmediate(Callable&& callable)
            {
                return Registration{
                    *this,
                    SourceKind::Immediate,
                    0,
                    std::chrono::milliseconds{0},
                    std::forward<Callable>(callable),
                };
            }

            // A handler may end its own registration, but not the one registered before it for the same signal.
            template <typename Callable>
                requires std::constructible_from<Handler, Callable>
            [[nodiscard]]
            Registration registerSignal(const int signal, Callable&& callable)
            {
                return Registration{
                    *this,
                    SourceKind::Signal,
                    signal,
                    std::chrono::milliseconds{0},
                    std::forward<Callable>(callable),
                };
            }

            // Returns whether any source was ready before the timeout.
            bool runOnce(std::chrono::milliseconds timeout);

            void run();

            void stop();
    };
} // namespace Consolpp::Event
