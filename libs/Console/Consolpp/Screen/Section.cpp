#include "Consolpp/Screen/Section.hpp"

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Screen/SectionStack.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"

#include <algorithm>
#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

namespace Consolpp::Screen
{
    namespace
    {
        std::vector<Markup::ParsedMarkup<Markup::ElementStyle>> parseLines(const Layout::Canvas& canvas)
        {
            std::vector<Markup::ParsedMarkup<Markup::ElementStyle>> lines;

            lines.reserve(canvas.getCount());

            for (std::size_t index = 0; index < canvas.getCount(); ++index) {
                lines.push_back(Styling::parseMarkup(canvas.getLine(index)));
            }

            return lines;
        }

        Layout::Canvas renderElement(const Layout::Element& element, const std::size_t availableWidth)
        {
            Layout::Canvas canvas;

            element.render(std::min(element.measure().natural, availableWidth), canvas);

            return canvas;
        }
    } // namespace

    Section::Section(SectionStack& stack, const Key /*key*/)
        : stack{&stack}
    {
    }

    void Section::append(const std::string_view markup)
    {
        this->stack->append(*this, Styling::parseMarkup(markup).splitLines());
    }

    void Section::append(const Layout::Canvas& canvas)
    {
        this->stack->append(*this, parseLines(canvas));
    }

    void Section::append(const Layout::Element& element)
    {
        this->append(renderElement(element, this->stack->output->getAvailableWidth()));
    }

    void Section::overwrite(const std::string_view markup)
    {
        this->stack->overwrite(*this, Styling::parseMarkup(markup).splitLines());
    }

    void Section::overwrite(const Layout::Canvas& canvas)
    {
        this->stack->overwrite(*this, parseLines(canvas));
    }

    void Section::overwrite(const Layout::Element& element)
    {
        this->overwrite(renderElement(element, this->stack->output->getAvailableWidth()));
    }

    void Section::clear()
    {
        this->stack->clear(*this);
    }

    void Section::setMaxHeight(const std::size_t maxHeight)
    {
        this->stack->setMaxHeight(*this, maxHeight);
    }

    std::span<const Markup::ParsedMarkup<Markup::ElementStyle>> Section::getVisibleLines() const
    {
        const std::size_t visible = this->maxHeight == 0 ? this->lines.size()
                                                         : std::min(this->maxHeight, this->lines.size());

        return std::span{this->lines}.last(visible);
    }
} // namespace Consolpp::Screen
