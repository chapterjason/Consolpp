#pragma once

#include "Consolpp/Markup/ParsedMarkupLineIterator.hpp"
#include "Consolpp/Text/WrapIterator.hpp"

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Markup
{
    template <typename Context>
    class ParsedMarkup;

    // The wrapped lines of parsed markup as slices; it must not outlive the markup.
    template <typename Context>
    class ParsedMarkupLineRange
    {
        private:
            const ParsedMarkup<Context>* markup;
            std::string_view text;
            std::size_t width;

        public:
            constexpr ParsedMarkupLineRange(const ParsedMarkup<Context>* const markup,
                                            const std::string_view text,
                                            const std::size_t width)
                : markup{markup},
                  text{text},
                  width{width}
            {
            }

            // Throws std::invalid_argument for a width of 0.
            [[nodiscard]]
            constexpr ParsedMarkupLineIterator<Context> begin() const
            {
                return ParsedMarkupLineIterator<Context>{this->markup, Text::WrapIterator{this->text, this->width}};
            }

            [[nodiscard]]
            static constexpr std::default_sentinel_t end()
            {
                return std::default_sentinel;
            }
    };
} // namespace Consolpp::Markup
