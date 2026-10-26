#include "Consolpp/Stream/InputStream.hpp"

#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/Console.hpp"
#include "Consolpp/Prompt/FormAnswer.hpp"
#include "Consolpp/Prompt/FormField.hpp"
#include "Consolpp/Prompt/InteractionDisabledError.hpp"
#include "Consolpp/Prompt/Selection.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Stream/ReadDescriptor.hpp"
#include "Consolpp/Stream/ReadError.hpp"
#include "Consolpp/Stream/ReadErrorKind.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <unistd.h>

namespace Consolpp::Stream
{
    namespace
    {
        std::string_view stripCarriageReturn(std::string_view line)
        {
            if (line.ends_with('\r')) {
                line.remove_suffix(1);
            }

            return line;
        }
    } // namespace

    InputStream::InputStream(const int descriptor)
        : descriptor{descriptor},
          interactive{isatty(descriptor) == 1}
    {
    }

    InputStream::InputStream(const int descriptor, OutputStream& output, Event::Loop& loop)
        : descriptor{descriptor},
          output{&output},
          loop{&loop},
          interactive{isatty(descriptor) == 1}
    {
    }

    InputStream::InputStream(const std::string_view text)
        : descriptor{-1},
          chunk{text},
          finished{true},
          interactive{false}
    {
    }

    InputStream::InputStream(const std::string_view text, OutputStream& output, Event::Loop& loop)
        : descriptor{-1},
          output{&output},
          loop{&loop},
          chunk{text},
          finished{true},
          interactive{false}
    {
    }

    Prompt::Console InputStream::createConsole() const
    {
        return Prompt::Console{*this->loop, this->descriptor, *this->output, InputStream::getKeyBindings()};
    }

    void InputStream::requireOutputAndLoop() const
    {
        if (this->output == nullptr || this->loop == nullptr) {
            throw std::logic_error{"Asking needs an input stream with an output and a loop"};
        }
    }

    void InputStream::keepUnconsumed(const std::string_view unconsumed)
    {
        auto* const end = std::ranges::copy(unconsumed, this->buffer.begin()).out;

        this->chunk = std::string_view{this->buffer.begin(), end};
    }

    std::vector<Prompt::FormAnswer> InputStream::askForm(const std::span<const Prompt::FormField> fields)
    {
        const std::string_view typedAhead = this->chunk;

        this->requireOutputAndLoop();

        this->chunk = std::string_view{};

        Prompt::Console console = this->createConsole();
        std::vector<Prompt::FormAnswer> answers = console.ask(fields, typedAhead);

        this->keepUnconsumed(console.takeUnconsumed());

        return answers;
    }

    std::string InputStream::ask(const Prompt::TextField& field)
    {
        const std::string_view typedAhead = this->chunk;

        this->requireOutputAndLoop();

        if (!this->interactive) {
            const std::optional<std::string_view> defaultValue = field.getOptions().defaultValue;

            if (!defaultValue.has_value()) {
                throw Prompt::InteractionDisabledError{};
            }

            return std::string{*defaultValue};
        }

        this->chunk = std::string_view{};

        Prompt::Console console = this->createConsole();
        auto answer = console.ask(field, typedAhead);

        this->keepUnconsumed(console.takeUnconsumed());

        return answer;
    }

    std::vector<std::size_t> InputStream::ask(const Prompt::ChoiceField& field)
    {
        const std::string_view typedAhead = this->chunk;
        const std::span<const std::size_t> defaultIndexes = field.getOptions().defaultIndexes;

        this->requireOutputAndLoop();

        if (!this->interactive && field.getOptions().selection == Prompt::Selection::Multiple) {
            return std::vector<std::size_t>{defaultIndexes.begin(), defaultIndexes.end()};
        }

        if (!this->interactive && defaultIndexes.empty()) {
            throw Prompt::InteractionDisabledError{};
        }

        if (!this->interactive) {
            return std::vector<std::size_t>{defaultIndexes.front()};
        }

        this->chunk = std::string_view{};

        Prompt::Console console = this->createConsole();
        auto answer = console.ask(field, typedAhead);

        this->keepUnconsumed(console.takeUnconsumed());

        return answer;
    }

    bool InputStream::ask(const Prompt::ConfirmationField& field)
    {
        const std::string_view typedAhead = this->chunk;

        this->requireOutputAndLoop();

        if (!this->interactive) {
            const std::optional<bool> defaultValue = field.getOptions().defaultValue;

            if (!defaultValue.has_value()) {
                throw Prompt::InteractionDisabledError{};
            }

            return *defaultValue;
        }

        this->chunk = std::string_view{};

        Prompt::Console console = this->createConsole();
        const bool answer = console.ask(field, typedAhead);

        this->keepUnconsumed(console.takeUnconsumed());

        return answer;
    }

    void InputStream::readChunk()
    {
        this->chunk = std::string_view{this->buffer.data(), Stream::readDescriptor(this->descriptor, this->buffer)};
        this->finished = this->chunk.empty();
    }

    void InputStream::appendPending(const std::string_view part)
    {
        if (part.empty()) {
            return;
        }

        if (this->pendingSize == 0) {
            this->overflow.clear();
        }

        if (this->overflow.empty() && this->pendingSize + part.size() <= this->pending.size()) {
            std::memcpy(std::next(this->pending.data(), static_cast<std::ptrdiff_t>(this->pendingSize)),
                        part.data(),
                        part.size());

            this->pendingSize += part.size();

            return;
        }

        if (this->overflow.empty()) {
            this->overflow.assign(this->pending.begin(),
                                  std::next(this->pending.begin(), static_cast<std::ptrdiff_t>(this->pendingSize)));
        }

        this->overflow.insert(this->overflow.end(), part.begin(), part.end());
        this->pendingSize += part.size();
    }

    std::string_view InputStream::takePending()
    {
        const char* const data = this->overflow.empty() ? this->pending.data() : this->overflow.data();
        const std::string_view line{data, this->pendingSize};

        this->pendingSize = 0;

        return line;
    }

    std::optional<std::string_view> InputStream::readLine()
    {
        while (true) {
            const std::size_t newline = this->chunk.find('\n');

            if (newline != std::string_view::npos) {
                const std::string_view part = this->chunk.substr(0, newline);

                this->chunk.remove_prefix(newline + 1);

                if (this->pendingSize == 0) {
                    return stripCarriageReturn(part);
                }

                this->appendPending(part);

                return stripCarriageReturn(this->takePending());
            }

            this->appendPending(this->chunk);
            this->chunk = {};

            if (this->finished) {
                if (this->pendingSize == 0) {
                    return std::nullopt;
                }

                return stripCarriageReturn(this->takePending());
            }

            this->readChunk();
        }
    }

    bool InputStream::skipWhitespace()
    {
        while (true) {
            const std::size_t start = this->chunk.find_first_not_of(InputStream::whitespace);

            if (start != std::string_view::npos) {
                this->chunk.remove_prefix(start);

                return true;
            }

            this->chunk = {};

            if (this->finished) {
                return false;
            }

            this->readChunk();
        }
    }

    std::optional<std::string_view> InputStream::readWord()
    {
        if (!this->skipWhitespace()) {
            return std::nullopt;
        }

        while (true) {
            const std::size_t end = this->chunk.find_first_of(InputStream::whitespace);

            if (end != std::string_view::npos) {
                const std::string_view part = this->chunk.substr(0, end);

                this->chunk.remove_prefix(end);

                if (this->pendingSize == 0) {
                    return part;
                }

                this->appendPending(part);

                return this->takePending();
            }

            this->appendPending(this->chunk);
            this->chunk = {};

            if (this->finished) {
                return this->takePending();
            }

            this->readChunk();
        }
    }

    std::string_view InputStream::requireWord()
    {
        const std::optional<std::string_view> word = this->readWord();

        if (!word.has_value()) {
            throw ReadError{ReadErrorKind::EndOfInput, std::string_view{}};
        }

        return *word;
    }

    InputStream& InputStream::operator>>(char& character)
    {
        if (!this->skipWhitespace()) {
            throw ReadError{ReadErrorKind::EndOfInput, std::string_view{}};
        }

        character = this->chunk.front();
        this->chunk.remove_prefix(1);

        return *this;
    }
} // namespace Consolpp::Stream
