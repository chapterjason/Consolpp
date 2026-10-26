#pragma once

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Layout/Extent.hpp"

#include <cstddef>
#include <span>

namespace Consolpp::Layout
{
    // Elements on top of each other, all at the full width.
    class Stack : public Element
    {
        private:
            std::span<const Element* const> children;
            std::size_t gap;

        public:
            // The children must outlive the stack. The gap is the blank lines between two children.
            Stack(std::span<const Element* const> children, std::size_t gap);

            [[nodiscard]]
            Extent measure() const override;

            void render(std::size_t width, Canvas& canvas) const override;
    };
} // namespace Consolpp::Layout
