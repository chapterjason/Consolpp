#include "Consolpp/Terminal/InputListener.hpp"

#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Keyboard/KeyCode.hpp"

#include <array>
#include <cerrno>
#include <chrono>
#include <string_view>
#include <system_error>
#include <vector>

#include <gtest/gtest.h>
#include <sys/types.h> // IWYU pragma: keep
#include <unistd.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Event::Loop;
    using Consolpp::Keyboard::Key;
    using Consolpp::Keyboard::KeyCode;
    using Consolpp::Terminal::InputListener;

    namespace
    {
        constexpr std::chrono::milliseconds escapeDelay{20};
        constexpr std::chrono::milliseconds wait{200};

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

                void write(const std::string_view bytes) const
                {
                    if (::write(std::get<1>(this->descriptors), bytes.data(), bytes.size())
                        != static_cast<ssize_t>(bytes.size())) {
                        throw std::system_error{errno, std::generic_category()};
                    }
                }

                void closeWriteEnd()
                {
                    int& writeEnd = std::get<1>(this->descriptors);

                    if (writeEnd >= 0) {
                        const int closed = close(writeEnd);

                        writeEnd = -1;

                        if (closed != 0) {
                            throw std::system_error{errno, std::generic_category()};
                        }
                    }
                }
        };
    } // namespace

    TEST(InputListenerTest, DeliversTheKeysOfTheBytesRead)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        std::vector<Key> keys;
        const std::vector<Key> expected{Key{.code = KeyCode::Character, .character = U'a'}, Key{.code = KeyCode::Up}};
        const InputListener reader{loop,
                                   pipe.getReadEnd(),
                                   [&keys](const Key& key) {
                                       keys.push_back(key);

                                       return true;
                                   },
                                   [](const std::string_view /*pasted*/) {
                                   },
                                   [] {
                                   },
                                   escapeDelay};

        pipe.write("a\x1b[A");

        // Act
        static_cast<void>(loop.runOnce(wait));

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputListenerTest, JoinsASequenceSplitOverTwoWrites)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        std::vector<Key> keys;
        const std::vector<Key> expected{Key{.code = KeyCode::Up}};
        const InputListener reader{loop,
                                   pipe.getReadEnd(),
                                   [&keys](const Key& key) {
                                       keys.push_back(key);

                                       return true;
                                   },
                                   [](const std::string_view /*pasted*/) {
                                   },
                                   [] {
                                   },
                                   escapeDelay};

        pipe.write("\x1b");
        static_cast<void>(loop.runOnce(wait));
        pipe.write("[A");

        // Act
        static_cast<void>(loop.runOnce(wait));

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputListenerTest, DeliversALoneEscapeAfterTheDelay)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        std::vector<Key> keys;
        const std::vector<Key> expected{Key{.code = KeyCode::Escape}};
        const InputListener reader{loop,
                                   pipe.getReadEnd(),
                                   [&keys](const Key& key) {
                                       keys.push_back(key);

                                       return true;
                                   },
                                   [](const std::string_view /*pasted*/) {
                                   },
                                   [] {
                                   },
                                   escapeDelay};

        pipe.write("\x1b");
        static_cast<void>(loop.runOnce(wait));

        // Act
        static_cast<void>(loop.runOnce(wait));

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputListenerTest, StopsReadingAtTheEndOfTheInput)
    {
        // Arrange
        Loop loop;
        Pipe pipe;
        int ends = 0;
        const InputListener reader{loop,
                                   pipe.getReadEnd(),
                                   [](const Key& /*key*/) {
                                       return true;
                                   },
                                   [](const std::string_view /*pasted*/) {
                                   },
                                   [&ends] {
                                       ++ends;
                                   },
                                   escapeDelay};

        pipe.closeWriteEnd();
        static_cast<void>(loop.runOnce(wait));

        // Act
        const bool readyAgain = loop.runOnce(std::chrono::milliseconds{20});

        // Assert
        EXPECT_EQ(ends, 1);
        EXPECT_FALSE(reader.isReading());
        EXPECT_FALSE(readyAgain);
    }

    TEST(InputListenerTest, KeepsTheBytesAfterTheKeyThatEndsTheListening)
    {
        // Arrange
        Loop loop;
        const Pipe pipe;
        std::vector<Key> keys;
        InputListener listener{loop,
                               pipe.getReadEnd(),
                               [&keys](const Key& key) {
                                   keys.push_back(key);

                                   return key.code != KeyCode::Enter;
                               },
                               [](const std::string_view /*pasted*/) {
                               },
                               [] {
                               },
                               escapeDelay};

        pipe.write("a\rbc");

        // Act
        static_cast<void>(loop.runOnce(wait));

        // Assert
        EXPECT_EQ(keys.size(), 2U);
        EXPECT_EQ(listener.takeUnconsumed(), "bc");
        EXPECT_FALSE(listener.isReading());
    }
} // namespace Consolpp::Tests::Integration::Terminal
