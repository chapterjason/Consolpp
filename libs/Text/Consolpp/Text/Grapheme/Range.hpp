#pragma once

#include "Consolpp/Text/Grapheme/Iterator.hpp"

#include <iterator>
#include <string_view>

namespace Consolpp::Text::Grapheme
{
    // The text must be valid UTF-8.
    class Range
    {
        private:
            std::string_view text;

        public:
            explicit constexpr Range(const std::string_view text)
                : text{text}
            {
            }

            [[nodiscard]]
            constexpr Iterator begin() const
            {
                return Iterator{this->text};
            }

            [[nodiscard]]
            static constexpr std::default_sentinel_t end()
            {
                return std::default_sentinel;
            }
    };
} // namespace Consolpp::Text::Grapheme
