#pragma once

#include "Consolpp/Markup/Attribute.hpp"
#include "Consolpp/Text/IsWhitespace.hpp"

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Markup
{
    // The attributes must be checked by the parser already.
    class AttributeIterator
    {
        private:
            std::string_view text;
            std::size_t position;
            Attribute current{};
            bool finished{false};

            constexpr void skip(const std::size_t count)
            {
                this->text.remove_prefix(count);
                this->position += count;
            }

            constexpr void skipWhitespace()
            {
                std::size_t count = 0;

                while (count < this->text.size() && Text::isWhitespace(this->text.at(count))) {
                    ++count;
                }

                this->skip(count);
            }

            constexpr void advance()
            {
                this->skipWhitespace();

                if (this->text.empty()) {
                    this->finished = true;

                    return;
                }

                std::size_t nameLength = 0;

                while (nameLength < this->text.size()
                       && !Text::isWhitespace(this->text.at(nameLength))
                       && this->text.at(nameLength) != '=') {
                    ++nameLength;
                }

                this->current.name = this->text.substr(0, nameLength);
                this->current.namePosition = this->position;
                this->skip(nameLength);
                this->skipWhitespace();
                this->skip(1);
                this->skipWhitespace();

                const char quote = this->text.front();
                const std::size_t valueLength = this->text.find(quote, 1) - 1;

                this->current.value = this->text.substr(1, valueLength);
                this->current.valuePosition = this->position + 1;
                this->skip(valueLength + 2);
            }

        public:
            // Named as std::iterator_traits requires.
            using value_type = Attribute;           // NOLINT(readability-identifier-naming)
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            // The position is where the text starts in the markup.
            constexpr AttributeIterator(const std::string_view text, const std::size_t position)
                : text{text},
                  position{position}
            {
                this->advance();
            }

            [[nodiscard]]
            constexpr const Attribute& operator*() const
            {
                return this->current;
            }

            constexpr AttributeIterator& operator++()
            {
                this->advance();

                return *this;
            }

            constexpr void operator++(int)
            {
                ++*this;
            }

            [[nodiscard]]
            constexpr bool operator==([[maybe_unused]] const std::default_sentinel_t sentinel) const
            {
                return this->finished;
            }
    };
} // namespace Consolpp::Markup
