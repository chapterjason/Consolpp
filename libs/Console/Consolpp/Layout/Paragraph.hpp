#pragma once

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Layout/Extent.hpp"
#include "Consolpp/Text/Alignment.hpp"

#include <cstddef>
#include <string_view>

namespace Consolpp::Layout
{
    // Markup wrapped at spaces and line breaks, words wider than the width cut between characters.
    class Paragraph : public Element
    {
        private:
            std::string_view markup;
            Text::Alignment alignment;
            std::size_t wordWidth{0};
            std::size_t lineWidth{0};

        public:
            // The markup must outlive the paragraph; it is measured with the registered tags, which must not change
            // while the paragraph is used. Throws like parseMarkup() for invalid markup.
            explicit Paragraph(std::string_view markup, Text::Alignment alignment = Text::Alignment::Left);

            // The minimum keeps the widest word whole.
            [[nodiscard]]
            Extent measure() const override;

            void render(std::size_t width, Canvas& canvas) const override;
    };
} // namespace Consolpp::Layout
