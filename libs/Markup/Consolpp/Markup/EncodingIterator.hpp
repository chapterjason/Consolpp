#pragma once

#include "Consolpp/Markup/Encode.hpp"

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Markup
{
    // Encodes every character written through it, like encode() does for a whole text.
    template <std::output_iterator<char> Output>
    class EncodingIterator
    {
        private:
            Output output;

        public:
            // Named as std::iterator_traits requires.
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            explicit constexpr EncodingIterator(Output output)
                : output{output}
            {
            }

            [[nodiscard]]
            constexpr EncodingIterator& operator*()
            {
                return *this;
            }

            constexpr EncodingIterator& operator=(const char character)
            {
                this->output = Markup::encode(std::string_view{&character, 1}, this->output);

                return *this;
            }

            constexpr EncodingIterator& operator++()
            {
                return *this;
            }

            constexpr EncodingIterator operator++(int)
            {
                return *this;
            }

            [[nodiscard]]
            constexpr Output base() const
            {
                return this->output;
            }
    };
} // namespace Consolpp::Markup
