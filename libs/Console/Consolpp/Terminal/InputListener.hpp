#pragma once

#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Event/Registration.hpp"
#include "Consolpp/Event/SourceKind.hpp"
#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Stream/ReadDescriptor.hpp"
#include "Consolpp/Terminal/InputDecoder.hpp"

#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace Consolpp::Terminal
{
    // Reads keys from the descriptor while the loop runs; a lone ESC becomes the Escape key once nothing follows it
    // within the escape delay. At the end of the input it calls onEnd and stops reading. onKey returns whether to go
    // on; once it does not, reading stops and the bytes read after that key are kept for takeUnconsumed(). Not
    // thread-safe, like Loop.
    template <std::predicate<const Keyboard::Key&> OnKey,
              std::invocable<std::string_view> OnPaste,
              std::invocable<> OnEnd>
    class InputListener
    {
        private:
            static constexpr std::size_t bufferCapacity = 256;

            Event::Loop* loop;
            int descriptor;
            bool listening{true};
            std::chrono::milliseconds escapeDelay;
            OnKey onKey;
            OnPaste onPaste;
            OnEnd onEnd;
            InputDecoder decoder;
            std::optional<Event::Registration> timeout;
            std::optional<Event::Registration> reading;
            std::string unconsumed;

            void deliverKey(const Keyboard::Key& key)
            {
                if (!this->onKey(key)) {
                    this->listening = false;
                }
            }

            void flush()
            {
                this->decoder.flush(
                    [this](const Keyboard::Key& key) {
                        this->deliverKey(key);
                    },
                    [this](const std::string_view text) {
                        this->onPaste(text);
                    });
            }

            void read()
            {
                std::array<char, bufferCapacity> buffer{};

                this->timeout.reset();

                const std::size_t received = Stream::readDescriptor(this->descriptor, buffer);

                if (received == 0) {
                    this->flush();
                    this->onEnd();
                    // Ends the registration whose handler is running, so nothing may follow.
                    this->reading.reset();

                    return;
                }

                this->decode(std::span{buffer}.first(received));

                if (!this->listening) {
                    // Ends the registration whose handler is running, so nothing may follow.
                    this->reading.reset();
                }
            }

        public:
            // Long enough for a sequence split over a slow connection, short enough for the Escape key to feel direct.
            static constexpr std::chrono::milliseconds defaultEscapeDelay{50};

            InputListener(Event::Loop& loop,
                          const int descriptor,
                          OnKey onKey,
                          OnPaste onPaste,
                          OnEnd onEnd,
                          const std::chrono::milliseconds escapeDelay = defaultEscapeDelay)
                : loop{&loop},
                  descriptor{descriptor},
                  escapeDelay{escapeDelay},
                  onKey{std::move(onKey)},
                  onPaste{std::move(onPaste)},
                  onEnd{std::move(onEnd)}
            {
                this->reading.emplace(loop,
                                      Event::SourceKind::Readable,
                                      descriptor,
                                      std::chrono::milliseconds{0},
                                      [this] {
                                          this->read();
                                      });
            }

            InputListener(const InputListener&) = delete;
            InputListener(InputListener&&) = delete;
            InputListener& operator=(const InputListener&) = delete;
            InputListener& operator=(InputListener&&) = delete;

            ~InputListener() = default;

            // Delivers the keys of bytes read elsewhere, such as input typed ahead, as if they had been read here.
            void decode(const std::span<const char> bytes)
            {
                this->timeout.reset();

                // Byte by byte, so that the bytes after the key that ends the listening stay unconsumed.
                for (std::size_t index = 0; index < bytes.size() && this->listening; ++index) {
                    this->decoder.decode(
                        bytes.subspan(index, 1),
                        [this](const Keyboard::Key& key) {
                            this->deliverKey(key);
                        },
                        [this](const std::string_view text) {
                            this->onPaste(text);
                        });

                    if (!this->listening) {
                        const std::span<const char> rest = bytes.subspan(index + 1);

                        this->unconsumed.append(rest.data(), rest.size());
                    }
                }

                if (this->listening && this->decoder.isPending()) {
                    this->timeout.emplace(*this->loop, Event::SourceKind::Timeout, 0, this->escapeDelay, [this] {
                        this->flush();
                    });
                }
            }

            [[nodiscard]]
            bool isReading() const
            {
                return this->reading.has_value();
            }

            // The bytes read after the key that ended the listening.
            [[nodiscard]]
            std::string takeUnconsumed()
            {
                return std::exchange(this->unconsumed, std::string{});
            }
    };
} // namespace Consolpp::Terminal
