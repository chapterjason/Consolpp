#pragma once

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/State.hpp"
#include "Consolpp/Prompt/ValidateTemplate.hpp"

#include <optional>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Prompt
{
    using namespace std::string_view_literals;

    // A yes or no, typed and then submitted.
    class ConfirmationField
    {
        private:
            static inline std::string_view questionTemplate{
                " <prompt-label>{label}</prompt-label> ({yes}/{no}){default}: {input}"sv,
            };
            static inline std::string_view defaultValueTemplate{" [<prompt-default>{value}</prompt-default>]"sv};
            static inline std::string_view yesTemplate{"yes"sv};
            static inline std::string_view noTemplate{"no"sv};
            static inline std::string_view answerTemplate{" <prompt-label>{label}</prompt-label>: {answer}"sv};

            ConfirmationFieldOptions options;
            std::optional<bool> typed;
            State state{State::Active};

        public:
            // Markup templates for every field of the class. Set them before fields are asked, as they are not
            // synchronized, and keep the markup alive as long as it is used.
            // {label}, which is markup, {yes}, {no}, {default} and the typed {input}; the cursor stays at the line's
            // end.
            static void setQuestionTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"label", "yes", "no", "default", "input"});

                ConfirmationField::questionTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getQuestionTemplate()
            {
                return ConfirmationField::questionTemplate;
            }

            // {value}
            static void setDefaultValueTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"value"});

                ConfirmationField::defaultValueTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getDefaultValueTemplate()
            {
                return ConfirmationField::defaultValueTemplate;
            }

            // The word for yes wherever it is shown.
            static void setYesTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {});

                ConfirmationField::yesTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getYesTemplate()
            {
                return ConfirmationField::yesTemplate;
            }

            // The word for no wherever it is shown.
            static void setNoTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {});

                ConfirmationField::noTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getNoTemplate()
            {
                return ConfirmationField::noTemplate;
            }

            // What stays after the answer: {label} and {answer}.
            static void setAnswerTemplate(const std::string_view markupTemplate)
            {
                Prompt::validateTemplate(markupTemplate, {"label", "answer"});

                ConfirmationField::answerTemplate = markupTemplate;
            }

            [[nodiscard]]
            static std::string_view getAnswerTemplate()
            {
                return ConfirmationField::answerTemplate;
            }

            explicit ConfirmationField(ConfirmationFieldOptions options);

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
            const ConfirmationFieldOptions& getOptions() const
            {
                return this->options;
            }

            [[nodiscard]]
            std::optional<bool> getTyped() const
            {
                return this->typed;
            }

            [[nodiscard]]
            State getState() const
            {
                return this->state;
            }

            // The typed answer, or the default value while nothing is typed; only reliable once submitted.
            [[nodiscard]]
            bool getValue() const
            {
                if (this->typed.has_value()) {
                    return *this->typed;
                }

                if (!this->options.defaultValue.has_value()) {
                    throw std::logic_error{"The confirmation field has neither a typed answer nor a default value"};
                }

                return *this->options.defaultValue;
            }
    };
} // namespace Consolpp::Prompt
