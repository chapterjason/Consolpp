#include "Consolpp/Prompt/Console.hpp"

#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Event/Registration.hpp"
#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/CancelledError.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/DrawConfirmationField.hpp"
#include "Consolpp/Prompt/DrawForm.hpp"
#include "Consolpp/Prompt/DrawTextField.hpp"
#include "Consolpp/Prompt/EraseFieldLines.hpp"
#include "Consolpp/Prompt/FindAction.hpp"
#include "Consolpp/Prompt/FormAnswer.hpp"
#include "Consolpp/Prompt/FormField.hpp"
#include "Consolpp/Prompt/FormatAnswerLine.hpp"
#include "Consolpp/Prompt/FormatChoiceLines.hpp"
#include "Consolpp/Prompt/FormatChoiceQuestion.hpp"
#include "Consolpp/Prompt/KeyBinding.hpp"
#include "Consolpp/Prompt/State.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Screen/Section.hpp"
#include "Consolpp/Screen/SectionStack.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/BracketedPaste.hpp"
#include "Consolpp/Terminal/HiddenCursor.hpp"
#include "Consolpp/Terminal/InputListener.hpp"
#include "Consolpp/Terminal/RawMode.hpp"

#include <chrono>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

// SIGWINCH is POSIX and not declared through <csignal>.
#include <signal.h> // IWYU pragma: keep NOLINT(modernize-deprecated-headers)

namespace Consolpp::Prompt
{
    namespace
    {
        // runOnce() instead of run() and stop(), which would also end a run() of the caller.
        constexpr std::chrono::hours waitSlice{1};

        State getState(const FormField& field)
        {
            return std::visit(
                [](const auto& visited) {
                    return visited.getState();
                },
                field);
        }

        // Applies the action to the field; a text field takes the key as typed when the action does not apply.
        template <typename Field>
        void applyKey(Field& field, const std::optional<Action> action, const Keyboard::Key& key)
        {
            const bool taken = action.has_value() && field.apply(*action);

            if constexpr (std::same_as<Field, TextField>) {
                const bool typable = key.code == Keyboard::KeyCode::Character
                                     && !key.modifiers.control
                                     && !key.modifiers.alt;

                if (!taken && typable) {
                    field.type(key.character);
                }
            }
        }

        void applyKey(FormField& field, const std::optional<Action> action, const Keyboard::Key& key)
        {
            std::visit(
                [action, &key](auto& visited) {
                    Prompt::applyKey(visited, action, key);
                },
                field);
        }

        // Delivers the keys and pastes of the descriptor until a key leaves the field inactive or the input ends, and
        // redraws after every key and on resize. Returns the bytes read after the last key.
        template <std::invocable<const Keyboard::Key&> TakeKey,
                  std::invocable<std::string_view> TakePaste,
                  std::invocable<> Redraw,
                  std::predicate<> IsActive>
        std::string listen(Event::Loop& loop,
                           const int descriptor,
                           const std::string_view typedAhead,
                           const TakeKey& takeKey,
                           TakePaste takePaste,
                           const Redraw& redraw,
                           const IsActive& isActive)
        {
            bool ended = false;
            Terminal::InputListener listener{
                loop,
                descriptor,
                [&takeKey, &redraw, &isActive](const Keyboard::Key& key) {
                    takeKey(key);
                    redraw();

                    return isActive();
                },
                std::move(takePaste),
                [&ended] {
                    ended = true;
                },
            };
            const Event::Registration resize = loop.registerSignal(SIGWINCH, [&redraw] {
                redraw();
            });

            listener.decode(typedAhead);

            while (isActive() && !ended) {
                static_cast<void>(loop.runOnce(waitSlice));
            }

            return listener.takeUnconsumed();
        }

        // Replaces the lines of the asked field by its answer line; throws CancelledError when it was not submitted.
        template <typename Field>
        void writeAnswerLine(Stream::OutputStream& output, const Field& asked, const std::size_t rowsAboveCursor)
        {
            Prompt::eraseFieldLines(output, rowsAboveCursor);

            if (asked.getState() != State::Submitted) {
                output.flush();

                throw CancelledError{};
            }

            std::string answer;

            Prompt::formatAnswerLine(asked, std::back_inserter(answer));
            output << answer;
            output.writeRaw("\n");
            output.flush();
        }

        FormAnswer getAnswer(const FormField& field)
        {
            return std::visit(
                [](const auto& visited) -> FormAnswer {
                    using Type = std::remove_cvref_t<decltype(visited)>;

                    if constexpr (std::same_as<Type, TextField>) {
                        return std::string{visited.getValue()};
                    } else if constexpr (std::same_as<Type, ChoiceField>) {
                        return visited.getChosenIndexes();
                    } else {
                        return visited.getValue();
                    }
                },
                field);
        }
    } // namespace

    Console::Console(Event::Loop& loop,
                     const int inputDescriptor,
                     Stream::OutputStream& output,
                     const std::span<const KeyBinding> keyBindings)
        : loop{&loop},
          inputDescriptor{inputDescriptor},
          output{&output},
          keyBindings{keyBindings}
    {
    }

    std::string Console::ask(const TextField& field, const std::string_view typedAhead)
    {
        Stream::OutputStream& output = *this->output;
        TextField asked = field;
        std::size_t rowsAboveCursor = 0;
        const Terminal::RawMode raw{this->inputDescriptor};
        const Terminal::BracketedPaste bracketedPaste{output};
        const auto redraw = [&asked, &output, &rowsAboveCursor] {
            rowsAboveCursor = Prompt::drawTextField(asked, output, rowsAboveCursor);
        };

        redraw();

        try {
            this->unconsumed = Prompt::listen(
                *this->loop,
                this->inputDescriptor,
                typedAhead,
                [this, &asked](const Keyboard::Key& key) {
                    Prompt::applyKey(asked, Prompt::findAction(this->keyBindings, key), key);
                },
                [&asked, &redraw](const std::string_view pasted) {
                    asked.paste(pasted);
                    redraw();
                },
                redraw,
                [&asked] {
                    return asked.getState() == State::Active;
                });
        } catch (...) {
            Prompt::eraseFieldLines(output, rowsAboveCursor);
            output.flush();

            throw;
        }

        Prompt::writeAnswerLine(output, asked, rowsAboveCursor);

        return std::string{asked.getValue()};
    }

    std::vector<std::size_t> Console::ask(const ChoiceField& field, const std::string_view typedAhead)
    {
        Stream::OutputStream& output = *this->output;
        ChoiceField asked = field;
        const std::string question = Prompt::formatChoiceQuestion(field);
        std::string markup;
        const Terminal::RawMode raw{this->inputDescriptor};
        const Terminal::HiddenCursor hiddenCursor{output};
        Screen::SectionStack sections{output};
        Screen::Section& section = sections.createSection();
        const auto draw = [&asked, &section, &markup, &question] {
            markup = question;
            markup += '\n';
            Prompt::formatChoiceLines(asked, std::back_inserter(markup));
            section.overwrite(markup);
        };

        draw();

        this->unconsumed = Prompt::listen(
            *this->loop,
            this->inputDescriptor,
            typedAhead,
            [this, &asked](const Keyboard::Key& key) {
                Prompt::applyKey(asked, Prompt::findAction(this->keyBindings, key), key);
            },
            // A choice field takes no text.
            [](const std::string_view /*pasted*/) {
            },
            draw,
            [&asked] {
                return asked.getState() == State::Active;
            });

        if (asked.getState() != State::Submitted) {
            section.clear();

            throw CancelledError{};
        }

        markup.clear();
        Prompt::formatAnswerLine(asked, std::back_inserter(markup));
        section.overwrite(markup);

        return asked.getChosenIndexes();
    }

    bool Console::ask(const ConfirmationField& field, const std::string_view typedAhead)
    {
        Stream::OutputStream& output = *this->output;
        ConfirmationField asked = field;
        std::size_t rowsAboveCursor = 0;
        const Terminal::RawMode raw{this->inputDescriptor};
        const auto redraw = [&asked, &output, &rowsAboveCursor] {
            rowsAboveCursor = Prompt::drawConfirmationField(asked, output, rowsAboveCursor);
        };

        redraw();

        try {
            this->unconsumed = Prompt::listen(
                *this->loop,
                this->inputDescriptor,
                typedAhead,
                [this, &asked](const Keyboard::Key& key) {
                    Prompt::applyKey(asked, Prompt::findAction(this->keyBindings, key), key);
                },
                // A confirmation takes no text.
                [](const std::string_view /*pasted*/) {
                },
                redraw,
                [&asked] {
                    return asked.getState() == State::Active;
                });
        } catch (...) {
            Prompt::eraseFieldLines(output, rowsAboveCursor);
            output.flush();

            throw;
        }

        Prompt::writeAnswerLine(output, asked, rowsAboveCursor);

        return asked.getValue();
    }

    std::vector<FormAnswer> Console::ask(const std::span<const FormField> fields, const std::string_view typedAhead)
    {
        Stream::OutputStream& output = *this->output;
        std::vector<FormField> asked{fields.begin(), fields.end()};
        std::size_t active = 0;
        std::size_t rowsAboveCursor = 0;
        const Terminal::RawMode raw{this->inputDescriptor};
        const Terminal::BracketedPaste bracketedPaste{output};
        std::optional<Terminal::HiddenCursor> hiddenCursor;
        const auto redraw = [&asked, &active, &hiddenCursor, &output, &rowsAboveCursor] {
            // Like asked alone, a choice field hides the cursor while it is active.
            if (!std::holds_alternative<ChoiceField>(asked.at(active))) {
                hiddenCursor.reset();
            } else if (!hiddenCursor.has_value()) {
                hiddenCursor.emplace(output);
            }

            rowsAboveCursor = Prompt::drawForm(asked, active, output, rowsAboveCursor);
        };
        // The fields before the active one are submitted, the active one and the ones after it are not.
        const auto takeKey = [this, &asked, &active](const Keyboard::Key& key) {
            const std::optional<Action> action = Prompt::findAction(this->keyBindings, key);

            if (action == Action::PreviousField && active > 0) {
                --active;
                std::visit(
                    [](auto& visited) {
                        visited.reopen();
                    },
                    asked.at(active));
            } else if (action != Action::PreviousField) {
                Prompt::applyKey(asked.at(active), action, key);
            }

            if (Prompt::getState(asked.at(active)) == State::Submitted && active + 1 < asked.size()) {
                ++active;
            }
        };

        redraw();

        try {
            this->unconsumed = Prompt::listen(
                *this->loop,
                this->inputDescriptor,
                typedAhead,
                takeKey,
                [&asked, &active, &redraw](const std::string_view pasted) {
                    if (auto* const text = std::get_if<TextField>(&asked.at(active))) {
                        text->paste(pasted);
                    }

                    redraw();
                },
                redraw,
                [&asked, &active] {
                    return Prompt::getState(asked.at(active)) == State::Active;
                });
        } catch (...) {
            hiddenCursor.reset();
            Prompt::eraseFieldLines(output, rowsAboveCursor);
            output.flush();

            throw;
        }

        hiddenCursor.reset();
        Prompt::eraseFieldLines(output, rowsAboveCursor);

        if (Prompt::getState(asked.back()) != State::Submitted) {
            output.flush();

            throw CancelledError{};
        }

        std::vector<FormAnswer> answers;

        for (const FormField& field : asked) {
            std::string answer;

            std::visit(
                [&answer](const auto& visited) {
                    Prompt::formatAnswerLine(visited, std::back_inserter(answer));
                },
                field);
            output << answer;
            output.writeRaw("\n");
            answers.push_back(Prompt::getAnswer(field));
        }

        output.flush();

        return answers;
    }
} // namespace Consolpp::Prompt
