#include "Consolpp/Prompt/ChoiceField.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/Selection.hpp"
#include "Consolpp/Prompt/State.hpp"

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace Consolpp::Prompt
{
    ChoiceField::ChoiceField(ChoiceFieldOptions options)
        : options{options},
          selected(this->options.choices.size(), false)
    {
        if (this->options.choices.empty()) {
            throw std::invalid_argument{"A choice field needs at least one choice"};
        }

        if (this->options.selection == Selection::Single && this->options.defaultIndexes.size() > 1) {
            throw std::invalid_argument{"A single selection takes at most one default index"};
        }

        for (const std::size_t index : this->options.defaultIndexes) {
            if (index >= this->options.choices.size()) {
                throw std::invalid_argument{"A default index is out of range"};
            }

            this->selected.at(index) = this->options.selection == Selection::Multiple;
        }

        if (!this->options.defaultIndexes.empty()) {
            this->highlighted = this->options.defaultIndexes.front();
        }
    }

    bool ChoiceField::apply(const Action action)
    {
        const std::size_t count = this->options.choices.size();

        if (this->state != State::Active) {
            return false;
        }

        switch (action) {
            case Action::MoveUp:
                this->highlighted = (this->highlighted + count - 1) % count;
                break;
            case Action::MoveDown:
                this->highlighted = (this->highlighted + 1) % count;
                break;
            case Action::Toggle:
                if (this->options.selection != Selection::Multiple) {
                    return false;
                }

                this->selected.at(this->highlighted) = !this->selected.at(this->highlighted);
                break;
            case Action::Submit:
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

    std::vector<std::size_t> ChoiceField::getChosenIndexes() const
    {
        std::vector<std::size_t> chosen;

        if (this->options.selection == Selection::Single) {
            chosen.push_back(this->highlighted);

            return chosen;
        }

        for (std::size_t index = 0; index < this->selected.size(); ++index) {
            if (this->selected.at(index)) {
                chosen.push_back(index);
            }
        }

        return chosen;
    }
} // namespace Consolpp::Prompt
