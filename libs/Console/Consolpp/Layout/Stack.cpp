#include "Consolpp/Layout/Stack.hpp"

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Layout/Extent.hpp"

#include <algorithm>
#include <cstddef>
#include <span>

namespace Consolpp::Layout
{
    Stack::Stack(const std::span<const Element* const> children, const std::size_t gap)
        : children{children},
          gap{gap}
    {
    }

    Extent Stack::measure() const
    {
        Extent extent{.minimum = 0, .natural = 0};

        for (const Element* const child : this->children) {
            const Extent childExtent = child->measure();

            extent.minimum = std::max(extent.minimum, childExtent.minimum);
            extent.natural = std::max(extent.natural, childExtent.natural);
        }

        return extent;
    }

    void Stack::render(const std::size_t width, Canvas& canvas) const
    {
        bool first = true;

        for (const Element* const child : this->children) {
            if (!first) {
                for (std::size_t index = 0; index < this->gap; ++index) {
                    canvas.addLine();
                    canvas.appendSpaces(width);
                }
            }

            child->render(width, canvas);
            first = false;
        }
    }
} // namespace Consolpp::Layout
