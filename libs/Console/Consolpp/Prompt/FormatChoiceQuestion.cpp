#include "Consolpp/Prompt/FormatChoiceQuestion.hpp"

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/FormatQuestionLine.hpp"

#include <cstddef>
#include <iterator>
#include <string>

namespace Consolpp::Prompt
{
    std::string formatChoiceQuestion(const ChoiceField& field)
    {
        std::string defaults;
        std::string line;

        for (const std::size_t index : field.getOptions().defaultIndexes) {
            defaults += defaults.empty() ? "" : ", ";
            defaults += field.getOptions().choices.subspan(index).front();
        }

        Prompt::formatQuestionLine(ChoiceField::getQuestionTemplate(),
                                   ChoiceField::getDefaultValueTemplate(),
                                   field.getOptions().label,
                                   defaults,
                                   std::back_inserter(line));

        return line;
    }
} // namespace Consolpp::Prompt
