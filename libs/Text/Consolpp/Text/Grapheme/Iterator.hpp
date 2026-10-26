#pragma once

#include "Consolpp/Text/Grapheme/FindEnd.hpp"

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Text::Grapheme
{
    class Iterator
    {
        private:
            std::string_view text;
            std::size_t start{0};
            std::size_t end{0};

            constexpr void findEnd()
            {
                this->end = this->start < this->text.size() ? Text::Grapheme::findEnd(this->text, this->start)
                                                            : this->start;
            }

        public:
            // Named as std::iterator_traits requires.
            using value_type = std::string_view;    // NOLINT(readability-identifier-naming)
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            explicit constexpr Iterator(const std::string_view text)
                : text{text}
            {
                this->findEnd();
            }

            [[nodiscard]]
            constexpr std::string_view operator*() const
            {
                return this->text.substr(this->start, this->end - this->start);
            }

            constexpr Iterator& operator++()
            {
                this->start = this->end;
                this->findEnd();

                return *this;
            }

            constexpr void operator++(int)
            {
                ++*this;
            }

            [[nodiscard]]
            constexpr bool operator==([[maybe_unused]] const std::default_sentinel_t sentinel) const
            {
                return this->start >= this->text.size();
            }
    };
} // namespace Consolpp::Text::Grapheme
