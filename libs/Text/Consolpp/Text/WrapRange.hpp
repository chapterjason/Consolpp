#pragma once

#include "Consolpp/Text/WrapIterator.hpp"

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Text
{
    // The lines of the text at the width; every text has at least one line.
    class WrapRange
    {
        private:
            std::string_view text;
            std::size_t width;

        public:
            constexpr WrapRange(const std::string_view text, const std::size_t width)
                : text{text},
                  width{width}
            {
            }

            [[nodiscard]]
            constexpr WrapIterator begin() const
            {
                return WrapIterator{this->text, this->width};
            }

            [[nodiscard]]
            static constexpr std::default_sentinel_t end()
            {
                return std::default_sentinel;
            }
    };
} // namespace Consolpp::Text
