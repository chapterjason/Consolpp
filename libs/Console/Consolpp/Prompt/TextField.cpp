#include "Consolpp/Prompt/TextField.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ControlCharacters.hpp"
#include "Consolpp/Prompt/State.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Text/Grapheme/FindEnd.hpp"
#include "Consolpp/Text/IsControl.hpp"
#include "Consolpp/Text/Utf8/CodePoint.hpp"
#include "Consolpp/Text/Utf8/Decode.hpp"
#include "Consolpp/Text/Utf8/Encode.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace Consolpp::Prompt
{
    namespace
    {
        Text::Utf8::CodePoint decodeOrReplace(const std::string_view text)
        {
            constexpr char32_t replacement = 0xFFFD;

            try {
                return Text::Utf8::decode(text);
            } catch (const std::invalid_argument&) {
                return Text::Utf8::CodePoint{.value = replacement, .length = 1};
            }
        }

        // Invalid UTF-8 becomes U+FFFD in every case, as the text stays valid UTF-8.
        std::string filterControlCharacters(const std::string_view text, const ControlCharacters handling)
        {
            constexpr std::string_view replacement{"\xEF\xBF\xBD"};
            std::string filtered;
            std::size_t position = 0;

            filtered.reserve(text.size());

            while (position < text.size()) {
                const Text::Utf8::CodePoint point = decodeOrReplace(text.substr(position));
                const bool lineBreak = point.value == U'\r' || point.value == U'\n';

                if (!Text::isControl(point.value) || handling == ControlCharacters::Keep) {
                    Text::Utf8::encode(point.value, std::back_inserter(filtered));
                } else if (handling == ControlCharacters::Replace && (lineBreak || point.value == U'\t')) {
                    filtered += ' ';
                } else if (handling == ControlCharacters::Replace) {
                    filtered += replacement;
                }

                // A CR LF line break becomes one space.
                if (handling == ControlCharacters::Replace
                    && point.value == U'\r'
                    && text.substr(position + 1).starts_with('\n')) {
                    ++position;
                }

                position += point.length;
            }

            return filtered;
        }
    } // namespace

    TextField::TextField(TextFieldOptions options)
        : options{std::move(options)}
    {
        this->findCompletions();
    }

    std::size_t TextField::findPreviousBoundary(const std::size_t position) const
    {
        std::size_t previous = 0;

        for (std::size_t current = 0; current < position; current = Text::Grapheme::findEnd(this->text, current)) {
            previous = current;
        }

        return previous;
    }

    std::size_t TextField::findNextBoundary(const std::size_t position) const
    {
        return position < this->text.size() ? Text::Grapheme::findEnd(this->text, position) : position;
    }

    void TextField::insert(const std::string_view inserted)
    {
        this->text.insert(this->cursor, inserted);
        this->cursor += inserted.size();
        this->findCompletions();
    }

    void TextField::eraseTowards(const std::size_t position)
    {
        const std::size_t start = std::min(position, this->cursor);
        const std::size_t end = std::max(position, this->cursor);

        if (start == end) {
            return;
        }

        this->text.erase(start, end - start);
        this->cursor = start;
        this->findCompletions();
    }

    void TextField::findCompletions()
    {
        this->completions.clear();
        this->suggestionIndex = 0;

        if (!this->options.autocompletion.has_value()) {
            return;
        }

        this->options.autocompletion->findCompletions(this->text, this->completions);
    }

    void TextField::moveSuggestion(const bool backwards)
    {
        const std::size_t count = this->completions.size();

        if (count == 0) {
            return;
        }

        this->suggestionIndex = backwards ? (this->suggestionIndex + count - 1) % count
                                          : (this->suggestionIndex + 1) % count;
    }

    void TextField::complete()
    {
        const std::string_view suggestion = this->getSuggestion();

        if (suggestion.empty()) {
            return;
        }

        this->text += suggestion;
        this->cursor = this->text.size();
        this->findCompletions();
    }

    std::string_view TextField::getSuggestion() const
    {
        if (this->completions.empty() || this->cursor != this->text.size()) {
            return std::string_view{};
        }

        const std::string_view shown = this->completions.at(this->suggestionIndex);

        return shown.starts_with(this->text) ? shown.substr(this->text.size()) : std::string_view{};
    }

    bool TextField::apply(const Action action)
    {
        if (this->state != State::Active) {
            return false;
        }

        switch (action) {
            case Action::MoveLeft:
                this->cursor = this->findPreviousBoundary(this->cursor);
                break;
            case Action::MoveRight:
                this->cursor = this->findNextBoundary(this->cursor);
                break;
            case Action::MoveUp:
                this->moveSuggestion(true);
                break;
            case Action::MoveDown:
                this->moveSuggestion(false);
                break;
            case Action::MoveToStart:
                this->cursor = 0;
                break;
            case Action::MoveToEnd:
                this->cursor = this->text.size();
                break;
            case Action::EraseBackward:
                this->eraseTowards(this->findPreviousBoundary(this->cursor));
                break;
            case Action::EraseForward:
                this->eraseTowards(this->findNextBoundary(this->cursor));
                break;
            case Action::Complete:
                this->complete();
                break;
            case Action::Submit:
                this->state = State::Submitted;
                break;
            case Action::Cancel:
                this->state = State::Cancelled;
                break;
            case Action::Toggle:
            case Action::Confirm:
            case Action::Deny:
            case Action::PreviousField:
                return false;
        }

        return true;
    }

    void TextField::type(const char32_t character)
    {
        std::string encoded;

        if (this->state != State::Active) {
            return;
        }

        Text::Utf8::encode(character, std::back_inserter(encoded));
        this->insert(encoded);
    }

    void TextField::paste(const std::string_view pasted)
    {
        if (this->state != State::Active) {
            return;
        }

        const std::string filtered = filterControlCharacters(pasted, TextField::controlCharacters);

        if (filtered.empty()) {
            return;
        }

        this->insert(filtered);
    }
} // namespace Consolpp::Prompt
