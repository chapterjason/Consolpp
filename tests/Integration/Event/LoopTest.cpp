#include "Consolpp/Event/Loop.hpp"

#include "Consolpp/Event/Registration.hpp"
#include "Consolpp/Event/SourceKind.hpp"

#include <array>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <optional>
#include <ratio>
#include <stdexcept>
#include <system_error>

#include <gtest/gtest.h>
#include <sys/time.h>
#include <sys/types.h> // IWYU pragma: keep
#include <unistd.h>

// POSIX sigaction is not declared through <csignal>.
#include <signal.h> // NOLINT(modernize-deprecated-headers)

namespace Consolpp::Tests::Integration::Event
{
    using Consolpp::Event::Loop;
    using Consolpp::Event::Registration;
    using Consolpp::Event::SourceKind;

    namespace
    {
        constexpr std::chrono::milliseconds dispatchWait{100};
        constexpr std::chrono::milliseconds quietWait{20};

        extern "C" void ignoreSignal(const int /*signal*/)
        {
        }

        class UnrelatedAlarm
        {
            private:
                struct sigaction previous{};

            public:
                explicit UnrelatedAlarm(const std::chrono::duration<suseconds_t, std::micro> delay)
                {
                    struct sigaction action{};
                    const itimerval timer{
                        .it_interval = {.tv_sec = 0, .tv_usec = 0},
                        .it_value = {.tv_sec = 0, .tv_usec = delay.count()},
                    };

                    action.sa_handler = ignoreSignal;
                    sigemptyset(&action.sa_mask);

                    if (sigaction(SIGALRM, &action, &this->previous) != 0
                        || setitimer(ITIMER_REAL, &timer, nullptr) != 0) {
                        throw std::system_error{errno, std::generic_category()};
                    }
                }

                UnrelatedAlarm(const UnrelatedAlarm&) = delete;
                UnrelatedAlarm(UnrelatedAlarm&&) = delete;
                UnrelatedAlarm& operator=(const UnrelatedAlarm&) = delete;
                UnrelatedAlarm& operator=(UnrelatedAlarm&&) = delete;

                ~UnrelatedAlarm()
                {
                    const itimerval disarmed{};

                    setitimer(ITIMER_REAL, &disarmed, nullptr);
                    sigaction(SIGALRM, &this->previous, nullptr);
                }
        };

        class Pipe
        {
            private:
                std::array<int, 2> descriptors{-1, -1};

            public:
                Pipe()
                {
                    if (pipe(this->descriptors.data()) != 0) {
                        throw std::system_error{errno, std::generic_category()};
                    }
                }

                Pipe(const Pipe&) = delete;
                Pipe(Pipe&&) = delete;
                Pipe& operator=(const Pipe&) = delete;
                Pipe& operator=(Pipe&&) = delete;

                ~Pipe()
                {
                    for (const int descriptor : this->descriptors) {
                        close(descriptor);
                    }
                }

                [[nodiscard]]
                int getReadEnd() const
                {
                    return std::get<0>(this->descriptors);
                }

                [[nodiscard]]
                int getWriteEnd() const
                {
                    return std::get<1>(this->descriptors);
                }

                void writeByte() const
                {
                    constexpr char byte = 'x';

                    if (write(this->getWriteEnd(), &byte, 1) != 1) {
                        throw std::system_error{errno, std::generic_category()};
                    }
                }
        };
    } // namespace

    TEST(LoopTest, RunOnceCallsTheHandlerOfARaisedSignal)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        const Registration registration = loop.registerSignal(SIGTERM, [&calls] {
            ++calls;
        });

        const int raised = std::raise(SIGTERM);

        // Act
        const bool ready = loop.runOnce(std::chrono::milliseconds{0});

        // Assert
        EXPECT_EQ(raised, 0);
        EXPECT_TRUE(ready);
        EXPECT_EQ(calls, 1);
    }

    TEST(LoopTest, RunOnceCallsEveryHandlerOfTheSignal)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        const Registration first = loop.registerSignal(SIGTERM, [&calls] {
            ++calls;
        });
        const Registration second = loop.registerSignal(SIGTERM, [&calls] {
            ++calls;
        });
        constexpr int expected = 2;

        const int raised = std::raise(SIGTERM);

        // Act
        static_cast<void>(loop.runOnce(std::chrono::milliseconds{0}));

        // Assert
        EXPECT_EQ(raised, 0);
        EXPECT_EQ(calls, expected);
    }

    TEST(LoopTest, RunOnceCallsOnlyTheHandlersOfTheRaisedSignal)
    {
        // Arrange
        Loop loop;
        int interrupts = 0;
        int terminations = 0;
        const Registration interrupt = loop.registerSignal(SIGINT, [&interrupts] {
            ++interrupts;
        });
        const Registration termination = loop.registerSignal(SIGTERM, [&terminations] {
            ++terminations;
        });

        const int raised = std::raise(SIGTERM);

        // Act
        static_cast<void>(loop.runOnce(std::chrono::milliseconds{0}));

        // Assert
        EXPECT_EQ(raised, 0);
        EXPECT_EQ(interrupts, 0);
        EXPECT_EQ(terminations, 1);
    }

    TEST(LoopTest, EndingTheLastRegistrationKeepsThePreviousAction)
    {
        // Arrange
        const auto previous = std::signal(SIGTERM, SIG_IGN);
        Loop loop;
        std::optional<Registration> registration;

        registration.emplace(loop, SourceKind::Signal, SIGTERM, std::chrono::milliseconds{0}, [] {
        });

        // Act
        registration.reset();

        // Assert
        EXPECT_EQ(std::signal(SIGTERM, previous), SIG_IGN);
    }

    TEST(LoopTest, RunOnceCallsTheHandlerOfAReadableDescriptor)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        int calls = 0;
        const Registration registration = loop.registerReadable(pipe.getReadEnd(), [&calls] {
            ++calls;
        });

        pipe.writeByte();

        // Act
        const bool ready = loop.runOnce(std::chrono::milliseconds{0});

        // Assert
        EXPECT_TRUE(ready);
        EXPECT_EQ(calls, 1);
    }

    TEST(LoopTest, RunOnceCallsTheHandlerOfAWritableDescriptor)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        int calls = 0;
        const Registration registration = loop.registerWritable(pipe.getWriteEnd(), [&calls] {
            ++calls;
        });

        // Act
        const bool ready = loop.runOnce(std::chrono::milliseconds{0});

        // Assert
        EXPECT_TRUE(ready);
        EXPECT_EQ(calls, 1);
    }

    TEST(LoopTest, RunOnceLeavesOutAnEndedDescriptorRegistration)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        int calls = 0;
        std::optional<Registration> registration;

        registration.emplace(loop, SourceKind::Readable, pipe.getReadEnd(), std::chrono::milliseconds{0}, [&calls] {
            ++calls;
        });
        pipe.writeByte();
        registration.reset();

        // Act
        const bool ready = loop.runOnce(std::chrono::milliseconds{0});

        // Assert
        EXPECT_FALSE(ready);
        EXPECT_EQ(calls, 0);
    }

    TEST(LoopTest, RunOnceCallsTheHandlerOfAnExpiredTimer)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        const Registration timer = loop.registerTimer(std::chrono::milliseconds{10}, [&calls] {
            ++calls;
        });

        // Act
        const bool ready = loop.runOnce(std::chrono::milliseconds{1000});

        // Assert
        EXPECT_TRUE(ready);
        EXPECT_EQ(calls, 1);
    }

    TEST(LoopTest, RunReturnsWhenAHandlerStopsTheLoop)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        const Registration timer = loop.registerTimer(std::chrono::milliseconds{1}, [&loop, &calls] {
            ++calls;
            loop.stop();
        });

        // Act
        loop.run();

        // Assert
        EXPECT_EQ(calls, 1);
    }

    TEST(LoopTest, RunReturnsAtOnceWhenTheLoopWasStoppedBefore)
    {
        // Arrange
        Loop loop;
        bool timedOut = false;
        const Registration timeout = loop.registerTimeout(dispatchWait, [&loop, &timedOut] {
            timedOut = true;
            loop.stop();
        });

        loop.stop();

        // Act
        loop.run();

        // Assert
        EXPECT_FALSE(timedOut);
    }

    TEST(LoopTest, RunKeepsRunningAfterAPreviousRunWasStopped)
    {
        // Arrange
        Loop loop;
        int calls = 0;

        loop.stop();
        loop.run();

        const Registration timeout = loop.registerTimeout(std::chrono::milliseconds{1}, [&loop, &calls] {
            ++calls;
            loop.stop();
        });

        // Act
        loop.run();

        // Assert
        EXPECT_EQ(calls, 1);
    }

    TEST(LoopTest, RunOnceReturnsFalseAfterTheTimeout)
    {
        // Arrange
        Loop loop;
        constexpr std::chrono::milliseconds timeout{10};

        // Act
        const bool ready = loop.runOnce(timeout);

        // Assert
        EXPECT_FALSE(ready);
    }

    TEST(LoopTest, RunOnceKeepsWaitingAfterAnUnrelatedSignal)
    {
        // Arrange
        Loop loop;
        constexpr std::chrono::milliseconds timeout{100};
        const UnrelatedAlarm alarm{std::chrono::milliseconds{20}};
        const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        // Act
        const bool ready = loop.runOnce(timeout);

        // Assert
        EXPECT_FALSE(ready);
        EXPECT_GE(std::chrono::steady_clock::now() - start, timeout);
    }

    TEST(LoopTest, WatchingADescriptorTwiceInOneDirectionThrows)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        const Registration first = loop.registerReadable(pipe.getReadEnd(), [] {
        });
        bool thrown = false;

        // Act
        try {
            const Registration second = loop.registerReadable(pipe.getReadEnd(), [] {
            });
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(LoopTest, RegisterTimerThrowsForAnIntervalThatIsNotPositive)
    {
        // Arrange
        Loop loop;
        bool thrown = false;

        // Act
        try {
            const Registration timer = loop.registerTimer(std::chrono::milliseconds{0}, [] {
            });
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(LoopTest, RegisterSignalThrowsForASignalOutOfRange)
    {
        // Arrange
        Loop loop;
        constexpr int signal = 1000;
        bool thrown = false;

        // Act
        try {
            const Registration registration = loop.registerSignal(signal, [] {
            });
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(LoopTest, ASecondInstanceThrows)
    {
        // Arrange
        const Loop loop;
        bool thrown = false;

        // Act
        try {
            const Loop second;
        } catch (const std::logic_error&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(LoopTest, ATimeoutFiresOnlyOnce)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        const Registration registration = loop.registerTimeout(std::chrono::milliseconds{1}, [&calls] {
            ++calls;
        });

        // Act
        static_cast<void>(loop.runOnce(dispatchWait));
        const bool readyAgain = loop.runOnce(quietWait);

        // Assert
        EXPECT_EQ(calls, 1);
        EXPECT_FALSE(readyAgain);
    }

    TEST(LoopTest, AReadableHandlerMayEndItsOwnRegistration)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        int calls = 0;
        std::optional<Registration> registration;

        registration.emplace(loop,
                             SourceKind::Readable,
                             pipe.getReadEnd(),
                             std::chrono::milliseconds{0},
                             [&calls, &registration] {
                                 ++calls;
                                 registration.reset();
                             });
        pipe.writeByte();

        // Act
        static_cast<void>(loop.runOnce(dispatchWait));
        const bool readyAgain = loop.runOnce(quietWait);

        // Assert
        EXPECT_EQ(calls, 1);
        EXPECT_FALSE(registration.has_value());
        EXPECT_FALSE(readyAgain);
    }

    TEST(LoopTest, ATimerHandlerMayEndItsOwnRegistration)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        std::optional<Registration> registration;

        registration.emplace(loop, SourceKind::Timer, 0, std::chrono::milliseconds{1}, [&calls, &registration] {
            ++calls;
            registration.reset();
        });

        // Act
        static_cast<void>(loop.runOnce(dispatchWait));
        const bool readyAgain = loop.runOnce(quietWait);

        // Assert
        EXPECT_EQ(calls, 1);
        EXPECT_FALSE(readyAgain);
    }

    TEST(LoopTest, RunOnceCallsAnImmediateWithoutWaitingForTheTimeout)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        const Registration registration = loop.registerImmediate([&calls] {
            ++calls;
        });
        constexpr std::chrono::milliseconds timeout{1000};
        const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        // Act
        const bool ready = loop.runOnce(timeout);

        // Assert
        EXPECT_TRUE(ready);
        EXPECT_EQ(calls, 1);
        EXPECT_LT(std::chrono::steady_clock::now() - start, timeout);
    }

    TEST(LoopTest, RunCallsAnImmediate)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        const Registration registration = loop.registerImmediate([&loop, &calls] {
            ++calls;
            loop.stop();
        });

        // Act
        loop.run();

        // Assert
        EXPECT_EQ(calls, 1);
    }

    TEST(LoopTest, AnImmediateFiresOnlyOnce)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        const Registration registration = loop.registerImmediate([&calls] {
            ++calls;
        });

        // Act
        static_cast<void>(loop.runOnce(std::chrono::milliseconds{0}));
        const bool readyAgain = loop.runOnce(quietWait);

        // Assert
        EXPECT_EQ(calls, 1);
        EXPECT_FALSE(readyAgain);
    }

    TEST(LoopTest, RunOnceLeavesOutAnEndedImmediate)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        std::optional<Registration> registration;

        registration.emplace(loop, SourceKind::Immediate, 0, std::chrono::milliseconds{0}, [&calls] {
            ++calls;
        });
        registration.reset();

        // Act
        const bool ready = loop.runOnce(std::chrono::milliseconds{0});

        // Assert
        EXPECT_FALSE(ready);
        EXPECT_EQ(calls, 0);
    }

    TEST(LoopTest, AnImmediateRegisteredByAnImmediateWaitsForTheNextRound)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        std::optional<Registration> second;
        const Registration first = loop.registerImmediate([&loop, &calls, &second] {
            second.emplace(loop, SourceKind::Immediate, 0, std::chrono::milliseconds{0}, [&calls] {
                ++calls;
            });
        });

        // Act
        static_cast<void>(loop.runOnce(std::chrono::milliseconds{0}));

        // Assert
        EXPECT_TRUE(second.has_value());
        EXPECT_EQ(calls, 0);
    }

    TEST(LoopTest, AnImmediateHandlerMayEndAnotherDueImmediate)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        std::optional<Registration> second;
        const Registration first = loop.registerImmediate([&second] {
            second.reset();
        });

        second.emplace(loop, SourceKind::Immediate, 0, std::chrono::milliseconds{0}, [&calls] {
            ++calls;
        });

        // Act
        static_cast<void>(loop.runOnce(std::chrono::milliseconds{0}));

        // Assert
        EXPECT_FALSE(second.has_value());
        EXPECT_EQ(calls, 0);
    }

    TEST(LoopTest, AnImmediateHandlerMayEndItsOwnRegistration)
    {
        // Arrange
        Loop loop;
        int calls = 0;
        std::optional<Registration> registration;

        registration.emplace(loop, SourceKind::Immediate, 0, std::chrono::milliseconds{0}, [&calls, &registration] {
            ++calls;
            registration.reset();
        });

        // Act
        static_cast<void>(loop.runOnce(std::chrono::milliseconds{0}));
        const bool readyAgain = loop.runOnce(quietWait);

        // Assert
        EXPECT_EQ(calls, 1);
        EXPECT_FALSE(registration.has_value());
        EXPECT_FALSE(readyAgain);
    }

    TEST(LoopTest, AnImmediateSkippedByAThrowingHandlerRunsInTheNextRound)
    {
        // Arrange
        Loop loop;
        int failingCalls = 0;
        int skippedCalls = 0;
        bool thrown = false;
        const Registration failing = loop.registerImmediate([&failingCalls] {
            ++failingCalls;

            throw std::runtime_error{"The handler failed"};
        });
        const Registration skipped = loop.registerImmediate([&skippedCalls] {
            ++skippedCalls;
        });

        try {
            static_cast<void>(loop.runOnce(std::chrono::milliseconds{0}));
        } catch (const std::runtime_error&) {
            thrown = true;
        }

        // Act
        const bool ready = loop.runOnce(std::chrono::milliseconds{0});

        // Assert
        EXPECT_TRUE(thrown);
        EXPECT_TRUE(ready);
        EXPECT_EQ(failingCalls, 1);
        EXPECT_EQ(skippedCalls, 1);
    }
} // namespace Consolpp::Tests::Integration::Event
