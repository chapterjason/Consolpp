#pragma once

#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Stream
{
    class InputStream;

    class LineIterator
    {
        private:
            InputStream* reader;
            std::string_view line;
            bool exhausted{false};

            void advance();

        public:
            // Named as std::iterator_traits requires.
            using value_type = std::string_view;    // NOLINT(readability-identifier-naming)
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            explicit LineIterator(InputStream& reader);

            [[nodiscard]]
            const std::string_view& operator*() const
            {
                return this->line;
            }

            LineIterator& operator++()
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
                return this->exhausted;
            }
    };
} // namespace Consolpp::Stream
