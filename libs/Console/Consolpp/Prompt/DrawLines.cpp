#include "Consolpp/Prompt/DrawLines.hpp"

#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Prompt/EraseFieldLines.hpp"
#include "Consolpp/Screen/CountRows.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <span>
#include <string>

namespace Consolpp::Prompt
{
    std::size_t drawLines(const std::span<const std::string> lines,
                          const std::size_t cursorLine,
                          const std::size_t cursorColumn,
                          Stream::OutputStream& output,
                          const std::size_t rowsAboveCursor)
    {
        const std::size_t columns = std::max<std::size_t>(output.getColumns(), 1);
        std::size_t rowsBeforeCursorLine = 0;
        std::size_t rowsAfterCursorRow = 0;
        std::size_t cursorRow = 0;

        Prompt::eraseFieldLines(output, rowsAboveCursor);

        for (std::size_t index = 0; index < lines.size(); ++index) {
            const Markup::ParsedMarkup<Markup::ElementStyle> line = Styling::parseMarkup(
                *std::next(lines.begin(), static_cast<std::ptrdiff_t>(index)));
            const std::size_t rows = Screen::countRows(line.getText(), columns);

            if (index > 0) {
                output.writeRaw("\n");
            }

            output << line;

            if (index < cursorLine) {
                rowsBeforeCursorLine += rows;
            } else if (index == cursorLine) {
                // A cursor right after a line that fills its last row stays in that row.
                cursorRow = std::min(cursorColumn / columns, rows - 1);
                rowsAfterCursorRow += rows - 1 - cursorRow;
            } else {
                rowsAfterCursorRow += rows;
            }
        }

        if (rowsAfterCursorRow > 0) {
            output.writeRaw("\x1b[");
            output << rowsAfterCursorRow;
            output.writeRaw("A");
        }

        output.writeRaw("\r");

        if (cursorColumn - (cursorRow * columns) > 0) {
            output.writeRaw("\x1b[");
            output << cursorColumn - (cursorRow * columns);
            output.writeRaw("C");
        }

        output.flush();

        return rowsBeforeCursorLine + cursorRow;
    }
} // namespace Consolpp::Prompt
