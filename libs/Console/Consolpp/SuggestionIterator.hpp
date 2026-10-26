#pragma once

#include <cstddef>
#include <iterator>
#include <span>
#include <string_view>

namespace Consolpp
{
    class SuggestionIterator
    {
        private:
            std::string_view name;
            std::span<const std::string_view> remaining;
            bool (*isSuggestion)(std::string_view, std::string_view);

            void skipRejected()
            {
                while (!this->remaining.empty() && !this->isSuggestion(this->name, this->remaining.front())) {
                    this->remaining = this->remaining.subspan(1);
                }
            }

            void advance()
            {
                this->remaining = this->remaining.subspan(1);

                this->skipRejected();
            }

        public:
            // Named as std::iterator_traits requires.
            using value_type = std::string_view;    // NOLINT(readability-identifier-naming)
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            SuggestionIterator(const std::string_view name,
                               const std::span<const std::string_view> candidates,
                               bool (*const isSuggestion)(std::string_view, std::string_view))
                : name{name},
                  remaining{candidates},
                  isSuggestion{isSuggestion}
            {
                this->skipRejected();
            }

            [[nodiscard]]
            const std::string_view& operator*() const
            {
                return this->remaining.front();
            }

            SuggestionIterator& operator++()
            {
                this->advance();

                return *this;
            }

            void operator++(int)
            {
                this->advance();
            }

            [[nodiscard]]
            bool operator==([[maybe_unused]] const std::default_sentinel_t sentinel) const
            {
                return this->remaining.empty();
            }
    };
} // namespace Consolpp
