#include "Consolpp/Layout/Paragraph.hpp"

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Extent.hpp"
#include "Consolpp/Styling/MarkupLine.hpp"
#include "Consolpp/Styling/MeasureWidestLine.hpp"
#include "Consolpp/Styling/MeasureWidestWord.hpp"
#include "Consolpp/Styling/WrapMarkup.hpp"
#include "Consolpp/Text/Alignment.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Layout
{
    Paragraph::Paragraph(const std::string_view markup, const Text::Alignment alignment)
        : markup{markup},
          alignment{alignment}
    {
        std::string text;

        this->wordWidth = Styling::measureWidestWord(markup, text);
        this->lineWidth = Styling::measureWidestLine(markup, text);
    }

    Extent Paragraph::measure() const
    {
        return Extent{.minimum = this->wordWidth, .natural = this->lineWidth};
    }

    void Paragraph::render(const std::size_t width, Canvas& canvas) const
    {
        std::string text;
        std::vector<Styling::MarkupLine> wrapped;

        Styling::wrapMarkup(this->markup, width, text, wrapped);

        for (const Styling::MarkupLine& line : wrapped) {
            const std::size_t free = width > line.width ? width - line.width : 0;
            std::size_t before = 0;

            if (this->alignment == Text::Alignment::Right) {
                before = free;
            } else if (this->alignment == Text::Alignment::Center) {
                before = free / 2;
            }

            canvas.addLine();
            canvas.appendSpaces(before);
            canvas.append(std::string_view{text}.substr(line.start, line.length), line.width);
            canvas.appendSpaces(free - before);
        }
    }
} // namespace Consolpp::Layout
