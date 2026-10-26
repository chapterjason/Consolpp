#include "Consolpp/Event/Loop.hpp"

#include "Consolpp/Event/DescriptorRegistrations.hpp"
#include "Consolpp/Event/Registration.hpp"
#include "Consolpp/Event/SourceKind.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <exception>
#include <iterator>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <vector>

#include <unistd.h>

#ifdef __linux__
#    include <sys/epoll.h>
#    include <sys/signalfd.h>
#    include <sys/timerfd.h>
#    include <sys/types.h>
#elif defined(__APPLE__)
#    include <fcntl.h>
#    include <sys/event.h>
#    include <sys/types.h> // IWYU pragma: keep
#else
#    error "Consolpp supports Linux and macOS only"
#endif

// POSIX sigset_t and sigaction are not declared through <csignal>.
#include <signal.h> // NOLINT(modernize-deprecated-headers)

namespace Consolpp::Event
{
    namespace
    {
        constexpr std::size_t eventCapacity = 64;

        // Signal handlers are process-wide, so only one loop can own them.
        std::atomic<bool> instanceExists{false}; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        std::chrono::milliseconds getRemaining(const std::chrono::steady_clock::time_point deadline)
        {
            return std::clamp(std::chrono::ceil<std::chrono::milliseconds>(deadline - std::chrono::steady_clock::now()),
                              std::chrono::milliseconds{0},
                              std::chrono::milliseconds{std::numeric_limits<int>::max()});
        }

        std::system_error createError()
        {
            return std::system_error{errno, std::generic_category()};
        }

        // Checked like at(), but ends the program instead of throwing, for functions that must not throw.
        template <typename Value>
        Value& getSlot(std::array<Value, signalLimit>& slots, const std::size_t signal) noexcept
        {
            if (signal >= slots.size()) {
                std::terminate();
            }

            return *std::next(slots.begin(), static_cast<std::ptrdiff_t>(signal));
        }

#ifdef __linux__
        constexpr std::uint32_t inputEvent = EPOLLIN;
        constexpr std::uint32_t outputEvent = EPOLLOUT;
        constexpr std::uint32_t hangUpEvent = EPOLLHUP;
        constexpr std::uint32_t errorEvent = EPOLLERR;

        sigset_t createSignalSet(const int signal)
        {
            sigset_t set{};

            sigemptyset(&set);
            sigaddset(&set, signal);

            return set;
        }

        epoll_event createEvent(const int descriptor, const std::uint32_t events)
        {
            return epoll_event{.events = events, .data = {.fd = descriptor}};
        }

        int getDescriptor(const epoll_event& event)
        {
            // epoll hands the registered data back in a union.
            return event.data.fd; // NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        void control(const int queue, const int operation, const int descriptor, const std::uint32_t events)
        {
            epoll_event event = createEvent(descriptor, events);

            if (epoll_ctl(queue, operation, descriptor, &event) != 0) {
                throw createError();
            }
        }

        std::uint32_t getEvents(const DescriptorRegistrations& registrations)
        {
            return (registrations.readable != nullptr ? inputEvent : 0U)
                   | (registrations.writable != nullptr ? outputEvent : 0U);
        }
#elif defined(__APPLE__)
        constexpr std::size_t wakeCapacity = 64;

        // The signal handler reaches these, so they must stay lock-free atomics.
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
        std::array<std::atomic<bool>, signalLimit> pendingSignals{};
        std::atomic<int> signalWriter{-1}; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        // kqueue only reports signals sent to the process, not those a thread sends to itself, e.g. raise().
        void forwardSignal(const int signal)
        {
            const int error = errno;
            constexpr char wake = 0;

            pendingSignals.at(static_cast<std::size_t>(signal)).store(true);

            // A full pipe already wakes the loop, so a failed write loses nothing.
            static_cast<void>(write(signalWriter.load(), &wake, 1));

            errno = error;
        }

        void prepareSignalEnd(const int descriptor)
        {
            if (fcntl(descriptor, F_SETFL, O_NONBLOCK) != 0 || fcntl(descriptor, F_SETFD, FD_CLOEXEC) != 0) {
                throw createError();
            }
        }

        std::int16_t getFilter(const SourceKind kind)
        {
            return kind == SourceKind::Readable ? EVFILT_READ : EVFILT_WRITE;
        }

        void change(const int queue,
                    const int identifier,
                    const std::int16_t filter,
                    const std::uint16_t flags,
                    const std::intptr_t data)
        {
            struct kevent event{};

            EV_SET(&event, static_cast<std::uintptr_t>(identifier), filter, flags, 0, data, nullptr);

            if (kevent(queue, &event, 1, nullptr, 0, nullptr) != 0) {
                throw createError();
            }
        }

        // An error here means the descriptor is already closed, which removes its events as well.
        void remove(const int queue, const int identifier, const std::int16_t filter) noexcept
        {
            struct kevent event{};

            EV_SET(&event, static_cast<std::uintptr_t>(identifier), filter, EV_DELETE, 0, 0, nullptr);
            kevent(queue, &event, 1, nullptr, 0, nullptr);
        }
#endif
    } // namespace

    Loop::Loop()
    {
        if (instanceExists.exchange(true)) {
            throw std::logic_error{"Only one Loop may exist per process"};
        }

#ifdef __linux__
        this->queue = epoll_create1(EPOLL_CLOEXEC);
#elif defined(__APPLE__)
        this->queue = kqueue();
#endif

        if (this->queue < 0) {
            const int error = errno;

            instanceExists.store(false);

            throw std::system_error{error, std::generic_category()};
        }

#ifdef __linux__
        sigemptyset(&this->signalMask);
        this->signals = signalfd(-1, &this->signalMask, SFD_NONBLOCK | SFD_CLOEXEC);

        try {
            if (this->signals < 0) {
                throw createError();
            }

            control(this->queue, EPOLL_CTL_ADD, this->signals, inputEvent);
        } catch (const std::system_error&) {
            if (this->signals >= 0) {
                close(this->signals);
            }

            close(this->queue);
            instanceExists.store(false);

            throw;
        }
#elif defined(__APPLE__)
        std::array<int, 2> ends{-1, -1};

        try {
            // Without pipe2(), a process another thread starts in between may inherit the ends.
            if (pipe(ends.data()) != 0) {
                throw createError();
            }

            prepareSignalEnd(std::get<0>(ends));
            prepareSignalEnd(std::get<1>(ends));
            change(this->queue, std::get<0>(ends), EVFILT_READ, EV_ADD, 0);
        } catch (const std::system_error&) {
            for (const int end : ends) {
                if (end >= 0) {
                    close(end);
                }
            }

            close(this->queue);
            instanceExists.store(false);

            throw;
        }

        this->signals = std::get<0>(ends);
        signalWriter.store(std::get<1>(ends));
#endif
    }

    Loop::~Loop()
    {
#ifdef __APPLE__
        for (std::size_t signal = 0; signal < signalLimit; ++signal) {
            if (getSlot(this->heads, signal) != nullptr) {
                sigaction(static_cast<int>(signal), &getSlot(this->previousActions, signal), nullptr);
                getSlot(pendingSignals, signal).store(false);
            }
        }

        close(signalWriter.exchange(-1));
#endif

        close(this->signals);
        close(this->queue);
        instanceExists.store(false);
    }

    void Loop::attach(Registration& registration)
    {
        switch (registration.kind) {
            case SourceKind::Readable:
            case SourceKind::Writable:
                this->attachDescriptor(registration);
                break;
            case SourceKind::Timer:
            case SourceKind::Timeout:
                this->attachTimer(registration);
                break;
            case SourceKind::Immediate:
                this->attachImmediate(registration);
                break;
            case SourceKind::Signal:
                this->attachSignal(registration);
                break;
        }
    }

    void Loop::detach(Registration& registration) noexcept
    {
        switch (registration.kind) {
            case SourceKind::Readable:
            case SourceKind::Writable:
                this->detachDescriptor(registration);
                break;
            case SourceKind::Timer:
            case SourceKind::Timeout:
                this->detachTimer(registration);
                break;
            case SourceKind::Immediate:
                this->detachImmediate(registration);
                break;
            case SourceKind::Signal:
                this->detachSignal(registration);
                break;
        }
    }

    void Loop::attachDescriptor(Registration& registration)
    {
        DescriptorRegistrations& registrations = this->descriptors[registration.target];
        const bool registered = registrations.readable != nullptr || registrations.writable != nullptr;
        Registration*& slot = registration.kind == SourceKind::Readable ? registrations.readable
                                                                        : registrations.writable;

        if (slot != nullptr) {
            throw std::invalid_argument{"The descriptor is already registered in this direction"};
        }

        slot = &registration;

        try {
#ifdef __linux__
            control(this->queue,
                    registered ? EPOLL_CTL_MOD : EPOLL_CTL_ADD,
                    registration.target,
                    getEvents(registrations));
#elif defined(__APPLE__)
            change(this->queue, registration.target, getFilter(registration.kind), EV_ADD, 0);
#endif
        } catch (const std::system_error&) {
            slot = nullptr;

            if (!registered) {
                this->descriptors.erase(registration.target);
            }

            throw;
        }
    }

    void Loop::detachDescriptor(Registration& registration) noexcept
    {
        const auto found = this->descriptors.find(registration.target);

        if (found == this->descriptors.end()) {
            std::terminate();
        }

        DescriptorRegistrations& registrations = found->second;

        (registration.kind == SourceKind::Readable ? registrations.readable : registrations.writable) = nullptr;

#ifdef __linux__
        const std::uint32_t events = getEvents(registrations);
        epoll_event event = createEvent(registration.target, events);

        // An error here means the descriptor is already closed, which removes it from the set as well.
        epoll_ctl(this->queue, events == 0 ? EPOLL_CTL_DEL : EPOLL_CTL_MOD, registration.target, &event);
#elif defined(__APPLE__)
        remove(this->queue, registration.target, getFilter(registration.kind));
#endif

        if (registrations.readable == nullptr && registrations.writable == nullptr) {
            this->descriptors.erase(found);
        }
    }

    void Loop::attachTimer(Registration& registration)
    {
        if (registration.interval <= std::chrono::milliseconds{0}) {
            throw std::invalid_argument{"A timer interval must be positive"};
        }

#ifdef __linux__
        const auto seconds = std::chrono::floor<std::chrono::seconds>(registration.interval);
        const std::chrono::nanoseconds rest = registration.interval - seconds;
        const timespec period{.tv_sec = seconds.count(), .tv_nsec = rest.count()};
        const timespec repetition = registration.kind == SourceKind::Timeout ? timespec{} : period;
        const itimerspec schedule{.it_interval = repetition, .it_value = period};
        const int timer = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

        if (timer < 0) {
            throw createError();
        }

        try {
            if (timerfd_settime(timer, 0, &schedule, nullptr) != 0) {
                throw createError();
            }

            this->timers.emplace(timer, &registration);
            control(this->queue, EPOLL_CTL_ADD, timer, inputEvent);
        } catch (...) {
            this->timers.erase(timer);
            close(timer);

            throw;
        }

        registration.target = timer;
#elif defined(__APPLE__)
        ++this->nextTimer;

        const int timer = this->nextTimer;

        this->timers.emplace(timer, &registration);

        try {
            const auto flags = static_cast<std::uint16_t>(registration.kind == SourceKind::Timeout ? EV_ADD | EV_ONESHOT
                                                                                                   : EV_ADD);

            change(this->queue, timer, EVFILT_TIMER, flags, registration.interval.count());
        } catch (const std::system_error&) {
            this->timers.erase(timer);

            throw;
        }

        registration.target = timer;
#endif
    }

    void Loop::detachTimer(Registration& registration) noexcept
    {
        this->timers.erase(registration.target);

#ifdef __linux__
        // Closing the timer also removes it from the set.
        close(registration.target);
#elif defined(__APPLE__)
        remove(this->queue, registration.target, EVFILT_TIMER);
#endif
    }

    void Loop::attachSignal(Registration& registration)
    {
        if (registration.target <= 0 || std::cmp_greater_equal(registration.target, signalLimit)) {
            throw std::invalid_argument{"The signal number is out of range"};
        }

        const auto index = static_cast<std::size_t>(registration.target);
        Registration*& head = this->heads.at(index);

        if (head == nullptr) {
#ifdef __linux__
            const sigset_t single = createSignalSet(registration.target);
            sigset_t previous{};
            const int blocked = pthread_sigmask(SIG_BLOCK, &single, &previous);

            if (blocked != 0) {
                throw std::system_error{blocked, std::generic_category()};
            }

            this->blockedBefore.at(index) = sigismember(&previous, registration.target) == 1;
            sigaddset(&this->signalMask, registration.target);

            if (signalfd(this->signals, &this->signalMask, 0) < 0) {
                const int error = errno;

                sigdelset(&this->signalMask, registration.target);

                if (!this->blockedBefore.at(index)) {
                    pthread_sigmask(SIG_UNBLOCK, &single, nullptr);
                }

                throw std::system_error{error, std::generic_category()};
            }
#elif defined(__APPLE__)
            struct sigaction action{};

            action.sa_handler = forwardSignal;
            // Calls the signal interrupts elsewhere in the program carry on instead of failing with EINTR.
            action.sa_flags = SA_RESTART;
            sigemptyset(&action.sa_mask);

            if (sigaction(registration.target, &action, &this->previousActions.at(index)) != 0) {
                throw createError();
            }
#endif
        }

        registration.next = head;

        if (head != nullptr) {
            head->previous = &registration;
        }

        head = &registration;
    }

    void Loop::detachSignal(Registration& registration) noexcept
    {
        const auto index = static_cast<std::size_t>(registration.target);
        Registration*& head = getSlot(this->heads, index);

        if (registration.previous != nullptr) {
            registration.previous->next = registration.next;
        } else {
            head = registration.next;
        }

        if (registration.next != nullptr) {
            registration.next->previous = registration.previous;
        }

        if (head != nullptr) {
            return;
        }

#ifdef __linux__
        const sigset_t single = createSignalSet(registration.target);

        sigdelset(&this->signalMask, registration.target);
        // Narrowing the mask of the signalfd the loop created cannot fail.
        signalfd(this->signals, &this->signalMask, 0);

        if (!getSlot(this->blockedBefore, index)) {
            constexpr timespec immediately{};

            // A pending instance belonged to the ended registration; unblocked, it would run its default action.
            sigtimedwait(&single, nullptr, &immediately);
            pthread_sigmask(SIG_UNBLOCK, &single, nullptr);
        }
#elif defined(__APPLE__)
        // Restoring an action that sigaction itself returned cannot fail.
        sigaction(registration.target, &getSlot(this->previousActions, index), nullptr);
        // Cleared only after the restore, as a pending instance belonged to the ended registration.
        getSlot(pendingSignals, index).store(false);
#endif
    }

    void Loop::attachImmediate(Registration& registration)
    {
        this->immediates.push_back(&registration);
    }

    void Loop::detachImmediate(Registration& registration) noexcept
    {
        // Not found once its handler was called.
        std::erase(this->immediates, &registration);
        std::erase(this->dueImmediates, &registration);
    }

    bool Loop::hasImmediates() const
    {
        return !this->immediates.empty() || !this->dueImmediates.empty();
    }

#ifdef __linux__
    void Loop::dispatchSignals()
    {
        while (true) {
            signalfd_siginfo information{};
            const ssize_t received = read(this->signals, &information, sizeof(information));

            if (received < 0 && errno == EAGAIN) {
                return;
            }

            if (received < 0) {
                throw createError();
            }

            if (std::cmp_less(information.ssi_signo, signalLimit)) {
                this->dispatchSignal(static_cast<int>(information.ssi_signo));
            }
        }
    }
#elif defined(__APPLE__)
    void Loop::dispatchSignals()
    {
        // Drained before the pending flags are taken, so a signal arriving in between wakes the loop again.
        while (true) {
            std::array<char, wakeCapacity> wakes{};
            const ssize_t received = read(this->signals, wakes.data(), wakes.size());

            if (received < 0 && errno == EAGAIN) {
                break;
            }

            if (received < 0) {
                throw createError();
            }
        }

        for (std::size_t signal = 1; signal < signalLimit; ++signal) {
            if (pendingSignals.at(signal).exchange(false)) {
                this->dispatchSignal(static_cast<int>(signal));
            }
        }
    }
#endif

    void Loop::dispatchDescriptor(const int descriptor, const bool readable, const bool writable)
    {
        if (readable) {
            const auto found = this->descriptors.find(descriptor);

            if (found != this->descriptors.end() && found->second.readable != nullptr) {
                found->second.readable->handler();
            }
        }

        // Looked up again, as the readable handler may have ended the writable registration.
        if (writable) {
            const auto found = this->descriptors.find(descriptor);

            if (found != this->descriptors.end() && found->second.writable != nullptr) {
                found->second.writable->handler();
            }
        }
    }

    void Loop::dispatchTimer(const int timer)
    {
        const auto found = this->timers.find(timer);

        if (found != this->timers.end()) {
            found->second->handler();
        }
    }

    void Loop::dispatchSignal(const int signal)
    {
        Registration* current = this->heads.at(static_cast<std::size_t>(signal));

        while (current != nullptr) {
            Registration* const following = current->next;

            current->handler();
            current = following;
        }
    }

    void Loop::dispatchImmediates()
    {
        // Ones registered by these handlers wait for the next round, so a handler registering itself cannot starve
        // the other sources.
        this->dueImmediates.insert(this->dueImmediates.end(), this->immediates.begin(), this->immediates.end());
        this->immediates.clear();

        while (!this->dueImmediates.empty()) {
            Registration* const registration = this->dueImmediates.front();

            this->dueImmediates.erase(this->dueImmediates.begin());

            registration->handler();
        }
    }

    bool Loop::waitAndDispatch(const std::optional<std::chrono::steady_clock::time_point> deadline)
    {
        const bool immediate = this->hasImmediates();
        // A due immediate does not wait for the other sources.
        const auto until = immediate ? std::optional{std::chrono::steady_clock::now()} : deadline;

#ifdef __linux__
        std::array<epoll_event, eventCapacity> events{};
        int count = 0;

        while (true) {
            const int timeout = until.has_value() ? static_cast<int>(getRemaining(*until).count()) : -1;

            count = epoll_wait(this->queue, events.data(), static_cast<int>(events.size()), timeout);

            if (count < 0 && errno == EINTR) {
                continue;
            }

            if (count < 0) {
                throw createError();
            }

            break;
        }

        for (const epoll_event& event : std::span{events}.first(static_cast<std::size_t>(count))) {
            const int descriptor = getDescriptor(event);

            if (descriptor == this->signals) {
                this->dispatchSignals();

                continue;
            }

            if (this->timers.contains(descriptor)) {
                std::uint64_t expirations = 0;
                const ssize_t received = read(descriptor, &expirations, sizeof(expirations));

                if (received < 0 && errno == EAGAIN) {
                    continue;
                }

                if (received < 0) {
                    throw createError();
                }

                this->dispatchTimer(descriptor);

                continue;
            }

            this->dispatchDescriptor(descriptor,
                                     (event.events & (inputEvent | hangUpEvent | errorEvent)) != 0,
                                     (event.events & (outputEvent | errorEvent)) != 0);
        }

        this->dispatchImmediates();

        return count > 0 || immediate;
#elif defined(__APPLE__)
        std::array<struct kevent, eventCapacity> events{};
        int count = 0;

        while (true) {
            std::optional<timespec> timeout;

            if (until.has_value()) {
                const std::chrono::milliseconds remaining = getRemaining(*until);
                const auto seconds = std::chrono::floor<std::chrono::seconds>(remaining);
                const std::chrono::nanoseconds rest = remaining - seconds;

                timeout = timespec{.tv_sec = seconds.count(), .tv_nsec = rest.count()};
            }

            count = kevent(this->queue,
                           nullptr,
                           0,
                           events.data(),
                           static_cast<int>(events.size()),
                           timeout.has_value() ? &*timeout : nullptr);

            if (count < 0 && errno == EINTR) {
                continue;
            }

            if (count < 0) {
                throw createError();
            }

            break;
        }

        for (const struct kevent& event : std::span{events}.first(static_cast<std::size_t>(count))) {
            const auto identifier = static_cast<int>(event.ident);

            if (event.filter == EVFILT_READ && identifier == this->signals) {
                this->dispatchSignals();

                continue;
            }

            switch (event.filter) {
                case EVFILT_READ:
                    this->dispatchDescriptor(identifier, true, false);
                    break;
                case EVFILT_WRITE:
                    this->dispatchDescriptor(identifier, false, true);
                    break;
                case EVFILT_TIMER:
                    this->dispatchTimer(identifier);
                    break;
                default:
                    break;
            }
        }

        this->dispatchImmediates();

        return count > 0 || immediate;
#endif
    }

    bool Loop::runOnce(const std::chrono::milliseconds timeout)
    {
        return this->waitAndDispatch(std::chrono::steady_clock::now() + timeout);
    }

    void Loop::run()
    {
        while (!this->stopped) {
            this->waitAndDispatch(std::nullopt);
        }

        this->stopped = false;
    }

    void Loop::stop()
    {
        this->stopped = true;
    }
} // namespace Consolpp::Event
