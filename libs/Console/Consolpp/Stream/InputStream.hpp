#pragma once

#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/Answer.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/Console.hpp"
#include "Consolpp/Prompt/DefaultKeyBindings.hpp"
#include "Consolpp/Prompt/Field.hpp"
#include "Consolpp/Prompt/Form.hpp"
#include "Consolpp/Prompt/FormAnswer.hpp"
#include "Consolpp/Prompt/FormField.hpp"
#include "Consolpp/Prompt/KeyBinding.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Stream/LineRange.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Stream/ReadError.hpp"
#include "Consolpp/Stream/ReadErrorKind.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace Consolpp::Stream
{
    class InputStream
    {
        private:
            static constexpr std::size_t capacity = 65536;
            static constexpr std::string_view whitespace{" \t\n\v\f\r"};

            std::array<char, capacity> buffer{};
            std::array<char, capacity> pending{};
            std::vector<char> overflow;
            int descriptor;
            OutputStream* output{nullptr};
            Event::Loop* loop{nullptr};
            std::string_view chunk;
            std::size_t pendingSize{0};
            bool finished{false};
            bool interactive;

            // Created on first use, so a failed allocation reaches the caller instead of the static initialization.
            static std::vector<Prompt::KeyBinding>& getStoredKeyBindings()
            {
                static std::vector<Prompt::KeyBinding> keyBindings{Prompt::defaultKeyBindings.begin(),
                                                                   Prompt::defaultKeyBindings.end()};

                return keyBindings;
            }

            [[nodiscard]]
            Prompt::Console createConsole() const;

            // The bytes a field read but did not take; the next read or field starts with them.
            void keepUnconsumed(std::string_view unconsumed);

            // Throws std::logic_error without output and loop.
            void requireOutputAndLoop() const;

            [[nodiscard]]
            std::vector<Prompt::FormAnswer> askForm(std::span<const Prompt::FormField> fields);

            // Replaces the chunk by the next bytes of the descriptor; none means the input is finished.
            void readChunk();

            void appendPending(const std::string_view part);

            [[nodiscard]]
            std::string_view takePending();

            [[nodiscard]]
            bool skipWhitespace();

            [[nodiscard]]
            std::string_view requireWord();

            void parseNumber(auto& value)
            {
                const std::string_view word = this->requireWord();
                const char* const first = word.data();
                const char* const end = std::next(first, static_cast<std::ptrdiff_t>(word.size()));
                const std::from_chars_result result = std::from_chars(first, end, value);

                if (result.ec == std::errc::result_out_of_range) {
                    throw ReadError{ReadErrorKind::OutOfRange, word};
                }

                if (result.ec != std::errc{} || result.ptr != end) {
                    throw ReadError{ReadErrorKind::InvalidValue, word};
                }
            }

        public:
            // The key bindings of every asked field. Set them before fields are asked, as they are not synchronized.
            static void setKeyBindings(const std::span<const Prompt::KeyBinding> bindings)
            {
                InputStream::getStoredKeyBindings().assign(bindings.begin(), bindings.end());
            }

            // The key triggers the action from now on, instead of what it was bound to before.
            static void bindKey(const Keyboard::Key& key, const Prompt::Action action)
            {
                std::vector<Prompt::KeyBinding>& keyBindings = InputStream::getStoredKeyBindings();
                const auto found = std::ranges::find(keyBindings, key, &Prompt::KeyBinding::key);

                if (found == keyBindings.end()) {
                    keyBindings.push_back(Prompt::KeyBinding{.key = key, .action = action});

                    return;
                }

                found->action = action;
            }

            [[nodiscard]]
            static std::span<const Prompt::KeyBinding> getKeyBindings()
            {
                return InputStream::getStoredKeyBindings();
            }

            explicit InputStream(const int descriptor);

            // With the output and loop, fields can be asked; both must outlive the stream.
            InputStream(const int descriptor, OutputStream& output, Event::Loop& loop);

            // Reads the text, which must outlive the stream, instead of a descriptor. It is no terminal, so interaction
            // starts disabled and asked fields take their defaults.
            explicit InputStream(const std::string_view text);

            InputStream(const std::string_view text, OutputStream& output, Event::Loop& loop);

            InputStream(const InputStream&) = delete;
            InputStream(InputStream&&) = delete;
            InputStream& operator=(const InputStream&) = delete;
            InputStream& operator=(InputStream&&) = delete;

            ~InputStream() = default;

            // Interaction is enabled at the start when the descriptor is a terminal. While it is disabled, asking a
            // field takes its default value, or throws Prompt::InteractionDisabledError without one.
            void enableInteraction()
            {
                this->interactive = true;
            }

            void disableInteraction()
            {
                this->interactive = false;
            }

            [[nodiscard]]
            bool isInteractive() const
            {
                return this->interactive;
            }

            // Asks the field on the terminal and returns the text; input already read but not taken counts as typed.
            // Throws std::logic_error without output and loop, and Prompt::CancelledError when the user cancels.
            std::string ask(const Prompt::TextField& field);

            // Returns the highlighted choice of a single selection, the selected ones otherwise; throws like the text
            // field.
            std::vector<std::size_t> ask(const Prompt::ChoiceField& field);

            // Returns the typed answer or the default value; throws like the text field.
            bool ask(const Prompt::ConfirmationField& field);

            // The answers in the order of the fields. While interaction is disabled, each field is answered like
            // asked alone.
            template <Prompt::Field... Fields>
            std::tuple<Prompt::Answer<Fields>...> ask(const Prompt::Form<Fields...>& form)
            {
                if (!this->interactive) {
                    return std::apply(
                        [this](const Fields&... fields) {
                            return std::tuple<Prompt::Answer<Fields>...>{this->ask(fields)...};
                        },
                        form.getFields());
                }

                const std::vector<Prompt::FormAnswer> answers = this->askForm(form.getFormFields());

                return [&answers]<std::size_t... Indexes>(std::index_sequence<Indexes...> /*indexes*/) {
                    return std::tuple<Prompt::Answer<Fields>...>{
                        std::get<Prompt::Answer<Fields>>(answers.at(Indexes))...};
                }(std::index_sequence_for<Fields...>{});
            }

            // Returned views point into buffers that the next read overwrites.
            [[nodiscard]]
            std::optional<std::string_view> readLine();

            [[nodiscard]]
            std::optional<std::string_view> readWord();

            [[nodiscard]]
            LineRange createLineRange()
            {
                return LineRange{*this};
            }

            InputStream& operator>>(std::string_view& word)
            {
                word = this->requireWord();

                return *this;
            }

            InputStream& operator>>(std::string& word)
            {
                word.assign(this->requireWord());

                return *this;
            }

            InputStream& operator>>(char& character);

            template <std::same_as<bool> Value>
            InputStream& operator>>(Value& value)
            {
                const std::string_view word = this->requireWord();

                if (word != "true" && word != "false") {
                    throw ReadError{ReadErrorKind::InvalidValue, word};
                }

                value = word == "true";

                return *this;
            }

            template <std::integral Value>
                requires(!std::same_as<Value, bool> && !std::same_as<Value, char>)
            InputStream& operator>>(Value& value)
            {
                this->parseNumber(value);

                return *this;
            }

            template <std::floating_point Value>
            InputStream& operator>>(Value& value)
            {
                this->parseNumber(value);

                return *this;
            }
    };
} // namespace Consolpp::Stream
