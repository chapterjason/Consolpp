#include "Consolpp/Event/Handler.hpp"

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Event
{
    using Consolpp::Event::Handler;

    namespace
    {
        class Probe
        {
            private:
                int* live;

            public:
                explicit Probe(int& live)
                    : live{&live}
                {
                    ++*this->live;
                }

                Probe(const Probe& other)
                    : live{other.live}
                {
                    ++*this->live;
                }

                Probe(Probe&& other) noexcept
                    : live{other.live}
                {
                    ++*this->live;
                }

                Probe& operator=(const Probe&) = delete;
                Probe& operator=(Probe&&) = delete;

                ~Probe()
                {
                    --*this->live;
                }

                void operator()() const
                {
                }
        };

        constexpr std::size_t oversized = 65;

        class Oversized
        {
            private:
                [[maybe_unused]]
                std::array<std::byte, oversized> bytes{};

            public:
                void operator()() const
                {
                }
        };
    } // namespace

    TEST(HandlerTest, CallsTheCallable)
    {
        // Arrange
        int calls = 0;
        Handler handler{[&calls] {
            ++calls;
        }};

        // Act
        handler();

        // Assert
        EXPECT_EQ(calls, 1);
    }

    TEST(HandlerTest, KeepsItsOwnCopyOfATemporary)
    {
        // Arrange
        constexpr int expected = 5;
        int result = 0;
        Handler handler{[value = expected, &result] {
            result = value;
        }};

        // Act
        handler();

        // Assert
        EXPECT_EQ(result, expected);
    }

    TEST(HandlerTest, DestroysTheCallableWithItself)
    {
        // Arrange
        int live = 0;
        std::optional<Handler> handler;

        handler.emplace(Probe{live});

        // Act
        handler.reset();

        // Assert
        EXPECT_EQ(live, 0);
    }

    TEST(HandlerTest, RejectsACallableLargerThanItsBuffer)
    {
        // Arrange
        using Callable = Oversized;

        // Act
        constexpr bool constructible = std::constructible_from<Handler, Callable>;

        // Assert
        EXPECT_FALSE(constructible);
    }
} // namespace Consolpp::Tests::Unit::Event
