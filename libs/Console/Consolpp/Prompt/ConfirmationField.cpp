#include "Consolpp/Prompt/ConfirmationField.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/State.hpp"

namespace Consolpp::Prompt
{
    ConfirmationField::ConfirmationField(ConfirmationFieldOptions options)
        : options{options}
    {
    }

    bool ConfirmationField::apply(const Action action)
    {
        if (this->state != State::Active) {
            return false;
        }

        switch (action) {
            case Action::Confirm:
                this->typed = true;
                break;
            case Action::Deny:
                this->typed = false;
                break;
            case Action::Submit:
                if (!this->typed.has_value() && !this->options.defaultValue.has_value()) {
                    return false;
                }

                this->state = State::Submitted;
                break;
            case Action::Cancel:
                this->state = State::Cancelled;
                break;
            default:
                return false;
        }

        return true;
    }
} // namespace Consolpp::Prompt
