#include "Consolpp/Layout/Canvas.hpp"

#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Styling/MarkupLine.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Layout
{
    void Canvas::addLine()
    {
        this->lines.push_back(Styling::MarkupLine{.start = this->text.size(), .length = 0, .width = 0});
    }

    void Canvas::append(const std::string_view markup, const std::size_t width)
    {
        Styling::MarkupLine& line = this->lines.back();

        this->text.append(markup);

        line.length += markup.size();
        line.width += width;
    }

    void Canvas::appendSpaces(const std::size_t count)
    {
        Styling::MarkupLine& line = this->lines.back();

        this->text.append(count, ' ');

        line.length += count;
        line.width += count;
    }

    void Canvas::appendText(const std::string_view text)
    {
        Styling::MarkupLine& line = this->lines.back();
        const std::size_t size = this->text.size();

        Markup::encode(text, std::back_inserter(this->text));

        line.length += this->text.size() - size;
        line.width += Text::measureDisplayWidth(text);
    }

    std::string_view Canvas::getLine(const std::size_t index) const
    {
        const Styling::MarkupLine& line = this->lines.at(index);

        return std::string_view{this->text}.substr(line.start, line.length);
    }

    std::size_t Canvas::getWidth(const std::size_t index) const
    {
        return this->lines.at(index).width;
    }

    void Canvas::clear()
    {
        this->text.clear();
        this->lines.clear();
    }
} // namespace Consolpp::Layout
