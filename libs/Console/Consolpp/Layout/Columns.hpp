#pragma once

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Layout/Extent.hpp"

#include <cstddef>
#include <span>

namespace Consolpp::Layout
{
    // Elements side by side, each at its natural width while there is room; narrower, the widest of them gives way
    // first, down to its minimum, then down to one column. Shorter elements are filled up with blank lines.
    class Columns : public Element
    {
        private:
            std::span<const Element* const> children;
            std::size_t gap;

        public:
            // The children must outlive the columns. The gap is the spaces between two children.
            Columns(std::span<const Element* const> children, std::size_t gap);

            [[nodiscard]]
            Extent measure() const override;

            void render(std::size_t width, Canvas& canvas) const override;
    };
} // namespace Consolpp::Layout
