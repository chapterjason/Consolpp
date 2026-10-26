#pragma once

#include "Consolpp/Markup/ParsedMarkupSlice.hpp"
#include "Consolpp/Text/WrapIterator.hpp"
#include "Consolpp/Text/WrappedLine.hpp"

#include <cstddef>
#include <iterator>

namespace Consolpp::Markup
{
    template <typename Context>
    class ParsedMarkup;

    template <typename Context>
    class ParsedMarkupLineIterator
    {
        private:
            const ParsedMarkup<Context>* markup;
            Text::WrapIterator lines;

        public:
            // Named as std::iterator_traits requires.
            using value_type = ParsedMarkupSlice<Context>; // NOLINT(readability-identifier-naming)
            using difference_type = std::ptrdiff_t;        // NOLINT(readability-identifier-naming)

            constexpr ParsedMarkupLineIterator(const ParsedMarkup<Context>* const markup,
                                               const Text::WrapIterator lines)
                : markup{markup},
                  lines{lines}
            {
            }

            [[nodiscard]]
            constexpr ParsedMarkupSlice<Context> operator*() const
            {
                const Text::WrappedLine& line = *this->lines;

                return ParsedMarkupSlice<Context>{.markup = this->markup, .start = line.start, .end = line.end};
            }

            constexpr ParsedMarkupLineIterator& operator++()
            {
                ++this->lines;

                return *this;
            }

            constexpr void operator++(int)
            {
                ++*this;
            }

            [[nodiscard]]
            constexpr bool operator==(const std::default_sentinel_t sentinel) const
            {
                return this->lines == sentinel;
            }
    };
} // namespace Consolpp::Markup
