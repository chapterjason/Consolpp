#include "Consolpp/Prompt/FormatTextField.hpp"

#include "Consolpp/Prompt/FormatInputLine.hpp"
#include "Consolpp/Prompt/FormatQuestionLine.hpp"
#include "Consolpp/Prompt/FormattedTextField.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Styling/MeasureMarkupWidth.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    FormattedTextField formatTextField(const TextField& field, const std::size_t columns)
    {
        constexpr std::string_view inputPlaceholder{"{input}"};
        const auto& options = field.getOptions();
        const std::string_view inputTemplate = TextField::getInputTemplate();
        const std::size_t inputPosition = inputTemplate.find(inputPlaceholder);
        const std::string suffix{inputTemplate.substr(inputPosition + inputPlaceholder.size())};
        FormattedTextField formatted{.question = {}, .input = std::string{inputTemplate.substr(0, inputPosition)}};
        std::string beforeCursor;
        std::string afterCursor;

        Prompt::formatQuestionLine(TextField::getQuestionTemplate(),
                                   TextField::getDefaultValueTemplate(),
                                   options.label,
                                   options.defaultValue.value_or(std::string_view{}),
                                   std::back_inserter(formatted.question));

        const std::size_t frameWidth = Text::measureDisplayWidth(
            Styling::parseMarkup(formatted.input + suffix).getText());

        Prompt::formatInputLine(field,
                                columns > frameWidth ? columns - frameWidth : 0,
                                std::back_inserter(beforeCursor),
                                std::back_inserter(afterCursor));
        formatted.input += beforeCursor;
        formatted.cursorColumn = Styling::measureMarkupWidth(formatted.input, formatted.input.size());
        formatted.input += afterCursor;
        formatted.input += suffix;

        return formatted;
    }
} // namespace Consolpp::Prompt
