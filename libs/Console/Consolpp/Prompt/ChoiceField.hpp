#pragma once

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/State.hpp"
#include "Consolpp/Prompt/ValidateTemplate.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

namespace Consolpp::Prompt
{
    using namespace std::string_view_literals;

    class ChoiceField
    {
        private:
            static inline std::string_view questionTemplate{" <prompt-label>{label}</prompt-label>{default}:"sv};
            static inline std::string_view defaultValueTemplate{" [<prompt-default>{value}</prompt-default>]"sv};
            static inline std::string_view highlightedTemplate{
                "  <prompt-highlight>&gt; {checkbox}{choice}</prompt-highlight>"sv,
            };
            static inline std::string_view choiceTemplate{"    {checkbox}{choice}"sv};
            static inline std::string_view selectedTemplate{"[x] "sv};
            static inline std::string_view unselectedTemplate{"[ ] "sv};
            static inline std::string_view answerTemplate{" <prompt-label>{label}</prompt-label>: {answer}"sv};

            ChoiceFieldOptions options;
            std::vector<bool> selected;
            std::size_t highlighted{0};
            State state{State::Active};

        public:
            // Markup templates for every field of the class; texts given as values are encoded first. Set them before
            // fields are asked, as they are not synchronized, and keep the markup alive as long as it is used.
            // {label}, which is markup, and {default}.
            static void setQuestionTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"label", "default"});

                ChoiceField::questionTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getQuestionTemplate()
            {
                return ChoiceField::questionTemplate;
            }

            // {value}, the default choices separated by commas.
            static void setDefaultValueTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"value"});

                ChoiceField::defaultValueTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getDefaultValueTemplate()
            {
                return ChoiceField::defaultValueTemplate;
            }

            // {checkbox} and {choice}.
            static void setHighlightedTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"checkbox", "choice"});

                ChoiceField::highlightedTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getHighlightedTemplate()
            {
                return ChoiceField::highlightedTemplate;
            }

            // {checkbox} and {choice} of every other choice.
            static void setChoiceTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"checkbox", "choice"});

                ChoiceField::choiceTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getChoiceTemplate()
            {
                return ChoiceField::choiceTemplate;
            }

            // In a multiple selection.
            static void setSelectedTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {});

                ChoiceField::selectedTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getSelectedTemplate()
            {
                return ChoiceField::selectedTemplate;
            }

            static void setUnselectedTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {});

                ChoiceField::unselectedTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getUnselectedTemplate()
            {
                return ChoiceField::unselectedTemplate;
            }

            // What stays after the answer: {label} and {answer}.
            static void setAnswerTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"label", "answer"});

                ChoiceField::answerTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getAnswerTemplate()
            {
                return ChoiceField::answerTemplate;
            }

            // Throws std::invalid_argument without choices, for a default index out of range and for several default
            // indexes of a single selection.
            explicit ChoiceField(ChoiceFieldOptions options);

            // Returns whether the field takes the action.
            bool apply(Action action);

            // Makes a submitted field active again, as a form does when it goes back to the field.
            void reopen()
            {
                if (this->state == State::Submitted) {
                    this->state = State::Active;
                }
            }

            [[nodiscard]]
            const ChoiceFieldOptions& getOptions() const
            {
                return this->options;
            }

            [[nodiscard]]
            std::size_t getHighlighted() const
            {
                return this->highlighted;
            }

            [[nodiscard]]
            bool isSelected(const std::size_t index) const
            {
                return this->selected.at(index);
            }

            [[nodiscard]]
            State getState() const
            {
                return this->state;
            }

            [[nodiscard]]
            std::vector<std::size_t> getChosenIndexes() const;
    };
} // namespace Consolpp::Prompt
