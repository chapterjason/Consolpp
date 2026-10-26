#pragma once

#include "Consolpp/Markup/AttributeIterator.hpp"

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Markup
{
    // The attributes must be checked by the parser already.
    class AttributeRange
    {
        private:
            std::string_view text;
            std::size_t position;

        public:
            // The position is where the text starts in the markup.
            constexpr AttributeRange(const std::string_view text, const std::size_t position)
                : text{text},
                  position{position}
            {
            }

            [[nodiscard]]
            constexpr AttributeIterator begin() const
            {
                return AttributeIterator{this->text, this->position};
            }

            [[nodiscard]]
            static constexpr std::default_sentinel_t end()
            {
                return std::default_sentinel;
            }
    };
} // namespace Consolpp::Markup
