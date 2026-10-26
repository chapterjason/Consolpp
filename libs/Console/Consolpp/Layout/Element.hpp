#pragma once

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Extent.hpp"

#include <cstddef>

namespace Consolpp::Layout
{
    // Something drawn into lines: a container first measures it, then gives it a width to draw at.
    class Element
    {
        public:
            Element() = default;
            Element(const Element&) = default;
            Element(Element&&) = default;
            Element& operator=(const Element&) = default;
            Element& operator=(Element&&) = default;

            virtual ~Element() = default;

            [[nodiscard]]
            virtual Extent measure() const = 0;

            // Adds lines of exactly the width; below the minimum the lines may get wider.
            virtual void render(std::size_t width, Canvas& canvas) const = 0;
    };
} // namespace Consolpp::Layout
