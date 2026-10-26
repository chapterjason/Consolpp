#pragma once

#include "Consolpp/SuggestionIterator.hpp"

#include <iterator>
#include <span>
#include <string_view>

namespace Consolpp
{
    class SuggestionRange
    {
        private:
            std::string_view name;
            std::span<const std::string_view> candidates;
            bool (*isSuggestion)(std::string_view, std::string_view);

        public:
            SuggestionRange(const std::string_view name,
                            const std::span<const std::string_view> candidates,
                            bool (*const isSuggestion)(std::string_view, std::string_view))
                : name{name},
                  candidates{candidates},
                  isSuggestion{isSuggestion}
            {
            }

            [[nodiscard]]
            SuggestionIterator begin() const
            {
                return SuggestionIterator{this->name, this->candidates, this->isSuggestion};
            }

            [[nodiscard]]
            static std::default_sentinel_t end()
            {
                return std::default_sentinel;
            }
    };
} // namespace Consolpp
