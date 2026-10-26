#include "Consolpp/Layout/Columns.hpp"

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Layout/Extent.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <span>
#include <vector>

namespace Consolpp::Layout
{
    namespace
    {
        // Narrows the widest child above its floor until the widths fit or every child is at its floor.
        void narrowTo(std::vector<std::size_t>& widths,
                      const std::size_t available,
                      const std::vector<std::size_t>& floors)
        {
            while (std::accumulate(widths.begin(), widths.end(), std::size_t{0}) > available) {
                std::size_t widest = widths.size();

                for (std::size_t index = 0; index < widths.size(); ++index) {
                    const bool above = widths.at(index) > floors.at(index);

                    if (above && (widest == widths.size() || widths.at(index) > widths.at(widest))) {
                        widest = index;
                    }
                }

                if (widest == widths.size()) {
                    return;
                }

                --widths.at(widest);
            }
        }
    } // namespace

    Columns::Columns(const std::span<const Element* const> children, const std::size_t gap)
        : children{children},
          gap{gap}
    {
    }

    Extent Columns::measure() const
    {
        const std::size_t gaps = this->children.empty() ? 0 : (this->children.size() - 1) * this->gap;
        Extent extent{.minimum = gaps, .natural = gaps};

        for (const Element* const child : this->children) {
            const Extent childExtent = child->measure();

            extent.minimum += childExtent.minimum;
            extent.natural += childExtent.natural;
        }

        return extent;
    }

    void Columns::render(const std::size_t width, Canvas& canvas) const
    {
        const std::size_t gaps = this->children.empty() ? 0 : (this->children.size() - 1) * this->gap;
        const std::size_t available = width > gaps ? width - gaps : 0;
        std::vector<std::size_t> widths;
        std::vector<std::size_t> floors;
        std::vector<Canvas> rendered(this->children.size());
        std::size_t height = 0;
        std::size_t drawn = gaps;

        for (const Element* const child : this->children) {
            const Extent extent = child->measure();

            widths.push_back(extent.natural);
            floors.push_back(std::max(extent.minimum, std::size_t{1}));
        }

        narrowTo(widths, available, floors);
        floors.assign(floors.size(), 1);
        narrowTo(widths, available, floors);

        for (std::size_t index = 0; index < this->children.size(); ++index) {
            (*std::next(this->children.begin(), static_cast<std::ptrdiff_t>(index)))
                ->render(widths.at(index), rendered.at(index));
            height = std::max(height, rendered.at(index).getCount());
            drawn += widths.at(index);
        }

        for (std::size_t row = 0; row < height; ++row) {
            canvas.addLine();

            for (std::size_t index = 0; index < this->children.size(); ++index) {
                const Canvas& child = rendered.at(index);
                const std::size_t childWidth = row < child.getCount() ? child.getWidth(row) : 0;

                if (index > 0) {
                    canvas.appendSpaces(this->gap);
                }

                if (row < child.getCount()) {
                    canvas.append(child.getLine(row), childWidth);
                }

                if (childWidth < widths.at(index)) {
                    canvas.appendSpaces(widths.at(index) - childWidth);
                }
            }

            if (drawn < width) {
                canvas.appendSpaces(width - drawn);
            }
        }
    }
} // namespace Consolpp::Layout
