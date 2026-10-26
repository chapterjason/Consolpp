#pragma once

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ControlCharacters.hpp"
#include "Consolpp/Prompt/State.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Prompt/ValidateTemplate.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Prompt
{
    using namespace std::string_view_literals;

    // The typed text and its cursor, changed key by key; the cursor stays on grapheme boundaries.
    class TextField
    {
        private:
            static inline std::string_view questionTemplate{
                " <prompt-label>{label}</prompt-label>{default}:"sv,
            };
            static inline std::string_view defaultValueTemplate{" [<prompt-default>{value}</prompt-default>]"sv};
            static inline std::string_view inputTemplate{" &gt; {input}"sv};
            static inline std::string_view suggestionTemplate{"<prompt-suggestion>{suggestion}</prompt-suggestion>"sv};
            static inline std::string_view answerTemplate{" <prompt-label>{label}</prompt-label>: {answer}"sv};
            static inline ControlCharacters controlCharacters{ControlCharacters::Drop};

            TextFieldOptions options;
            std::string text;
            std::size_t cursor{0};
            State state{State::Active};
            std::vector<std::string> completions;
            // The completion shown muted; Down and Up move it through the completions.
            std::size_t suggestionIndex{0};

            [[nodiscard]]
            std::size_t findPreviousBoundary(std::size_t position) const;

            [[nodiscard]]
            std::size_t findNextBoundary(std::size_t position) const;

            void insert(std::string_view inserted);

            void eraseTowards(std::size_t position);

            void findCompletions();

            void moveSuggestion(bool backwards);

            void complete();

        public:
            // Markup templates for every field of the class; texts typed or given as values are encoded first. Set them
            // before fields are asked, as they are not synchronized, and keep the markup alive as long as it is used.
            // {label}, which is markup, and {default}.
            static void setQuestionTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"label", "default"});

                TextField::questionTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getQuestionTemplate()
            {
                return TextField::questionTemplate;
            }

            // {value}
            static void setDefaultValueTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"value"});

                TextField::defaultValueTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getDefaultValueTemplate()
            {
                return TextField::defaultValueTemplate;
            }

            // {input}, exactly once.
            static void setInputTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"input"}, {"input"});

                TextField::inputTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getInputTemplate()
            {
                return TextField::inputTemplate;
            }

            // {suggestion}
            static void setSuggestionTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"suggestion"});

                TextField::suggestionTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getSuggestionTemplate()
            {
                return TextField::suggestionTemplate;
            }

            // What stays after the answer: {label} and {answer}.
            static void setAnswerTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"label", "answer"});

                TextField::answerTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getAnswerTemplate()
            {
                return TextField::answerTemplate;
            }

            // Set it before fields are asked, as it is not synchronized.
            static void setControlCharacters(const ControlCharacters handling)
            {
                TextField::controlCharacters = handling;
            }

            [[nodiscard]]
            static ControlCharacters getControlCharacters()
            {
                return TextField::controlCharacters;
            }

            explicit TextField(TextFieldOptions options);

            // Returns whether the field takes the action.
            bool apply(Action action);

            // Makes a submitted field active again, as a form does when it goes back to the field.
            void reopen()
            {
                if (this->state == State::Submitted) {
                    this->state = State::Active;
                }
            }

            void type(char32_t character);

            void paste(std::string_view pasted);

            [[nodiscard]]
            const TextFieldOptions& getOptions() const
            {
                return this->options;
            }

            [[nodiscard]]
            std::string_view getText() const
            {
                return this->text;
            }

            // A byte offset into the text.
            [[nodiscard]]
            std::size_t getCursor() const
            {
                return this->cursor;
            }

            [[nodiscard]]
            State getState() const
            {
                return this->state;
            }

            // The text, or the default value while the text is empty.
            [[nodiscard]]
            std::string_view getValue() const
            {
                return this->text.empty() ? this->options.defaultValue.value_or(std::string_view{}) : this->text;
            }

            // The rest of the shown completion after the text; empty without one.
            [[nodiscard]]
            std::string_view getSuggestion() const;
    };
} // namespace Consolpp::Prompt
