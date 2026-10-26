#include "Consolpp/Prompt/FormatConfirmationField.hpp"

#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/FormatTemplate.hpp"
#include "Consolpp/Prompt/Replacement.hpp"

#include <iterator>
#include <optional>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    std::string formatConfirmationField(const ConfirmationField& field)
    {
        const std::optional<bool> defaultValue = field.getOptions().defaultValue;
        std::string defaultPart;
        std::string line;
        std::string_view input;

        if (field.getTyped().has_value()) {
            input = *field.getTyped() ? ConfirmationField::getYesTemplate() : ConfirmationField::getNoTemplate();
        }

        if (defaultValue.has_value()) {
            Prompt::formatTemplate(ConfirmationField::getDefaultValueTemplate(),
                                   {
                                       Replacement{
                                           .name = "value",
                                           .value = *defaultValue ? ConfirmationField::getYesTemplate()
                                                                  : ConfirmationField::getNoTemplate(),
                                       },
                                   },
                                   std::back_inserter(defaultPart));
        }

        Prompt::formatTemplate(ConfirmationField::getQuestionTemplate(),
                               {
                                   Replacement{.name = "label", .value = field.getOptions().label},
                                   Replacement{.name = "yes", .value = ConfirmationField::getYesTemplate()},
                                   Replacement{.name = "no", .value = ConfirmationField::getNoTemplate()},
                                   Replacement{.name = "default", .value = defaultPart},
                                   Replacement{.name = "input", .value = input},
                               },
                               std::back_inserter(line));

        return line;
    }
} // namespace Consolpp::Prompt
