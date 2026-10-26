#include "Consolpp/Prompt/DrawForm.hpp"

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/DrawLines.hpp"
#include "Consolpp/Prompt/FormField.hpp"
#include "Consolpp/Prompt/FormatAnswerLine.hpp"
#include "Consolpp/Prompt/FormatChoiceLines.hpp"
#include "Consolpp/Prompt/FormatChoiceQuestion.hpp"
#include "Consolpp/Prompt/FormatConfirmationField.hpp"
#include "Consolpp/Prompt/FormatTextField.hpp"
#include "Consolpp/Prompt/FormattedTextField.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Styling/MeasureMarkupWidth.hpp"
#include "Consolpp/Text/Splitter.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace Consolpp::Prompt
{
    namespace
    {
        std::string formatQuestion(const FormField& field, const std::size_t columns)
        {
            return std::visit(
                [columns](const auto& visited) {
                    using Type = std::remove_cvref_t<decltype(visited)>;

                    if constexpr (std::same_as<Type, TextField>) {
                        return Prompt::formatTextField(visited, columns).question;
                    } else if constexpr (std::same_as<Type, ChoiceField>) {
                        return Prompt::formatChoiceQuestion(visited);
                    } else {
                        return Prompt::formatConfirmationField(visited);
                    }
                },
                field);
        }
    } // namespace

    std::size_t drawForm(const std::span<const FormField> fields,
                         const std::size_t active,
                         Stream::OutputStream& output,
                         const std::size_t rowsAboveCursor)
    {
        const std::size_t columns = std::max<std::size_t>(output.getColumns(), 1);
        std::vector<std::string> lines;
        std::size_t cursorLine = 0;
        std::size_t cursorColumn = 0;

        for (std::size_t index = 0; index < fields.size(); ++index) {
            const FormField& field = *std::next(fields.begin(), static_cast<std::ptrdiff_t>(index));

            if (index < active) {
                std::string line;

                std::visit(
                    [&line](const auto& visited) {
                        Prompt::formatAnswerLine(visited, std::back_inserter(line));
                    },
                    field);
                lines.push_back(std::move(line));
            } else if (index > active) {
                lines.push_back(formatQuestion(field, columns));
            } else if (const auto* const text = std::get_if<TextField>(&field)) {
                FormattedTextField formatted = Prompt::formatTextField(*text, columns);

                lines.push_back(std::move(formatted.question));
                cursorLine = lines.size();
                cursorColumn = formatted.cursorColumn;
                lines.push_back(std::move(formatted.input));
            } else if (const auto* const choice = std::get_if<ChoiceField>(&field)) {
                std::string choices;

                // The cursor is hidden while a choice field is active, so it waits at the start of the question.
                cursorLine = lines.size();
                cursorColumn = 0;
                lines.push_back(Prompt::formatChoiceQuestion(*choice));
                Prompt::formatChoiceLines(*choice, std::back_inserter(choices));

                for (Text::Splitter choiceLines{choices, '\n'}; choiceLines.hasNext();) {
                    lines.emplace_back(choiceLines.takeNext());
                }
            } else {
                std::string line = Prompt::formatConfirmationField(std::get<ConfirmationField>(field));

                cursorLine = lines.size();
                cursorColumn = Styling::measureMarkupWidth(line, line.size());
                lines.push_back(std::move(line));
            }
        }

        return Prompt::drawLines(lines, cursorLine, cursorColumn, output, rowsAboveCursor);
    }
} // namespace Consolpp::Prompt
