#include "Consolpp/Prompt/DrawConfirmationField.hpp"

#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/EraseFieldLines.hpp"
#include "Consolpp/Prompt/FormatConfirmationField.hpp"
#include "Consolpp/Screen/CountRows.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"

#include <algorithm>
#include <cstddef>

namespace Consolpp::Prompt
{
    std::size_t drawConfirmationField(const ConfirmationField& field,
                                      Stream::OutputStream& output,
                                      const std::size_t rowsAboveCursor)
    {
        const std::size_t columns = std::max<std::size_t>(output.getColumns(), 1);
        const Markup::ParsedMarkup<Markup::ElementStyle> parsed = Styling::parseMarkup(
            Prompt::formatConfirmationField(field));

        Prompt::eraseFieldLines(output, rowsAboveCursor);
        output << parsed;
        output.flush();

        return Screen::countRows(parsed.getText(), columns) - 1;
    }
} // namespace Consolpp::Prompt
