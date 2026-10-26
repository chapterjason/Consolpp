#pragma once

#include "Consolpp/Text/Grapheme/Range.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"
#include "Consolpp/Text/MeasureGraphemeDisplayWidth.hpp"
#include "Consolpp/Text/WrappedLine.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Text
{
    // Breaks at spaces and cuts words wider than a line between grapheme clusters; the spaces at a break are dropped.
    class WrapIterator
    {
        private:
            std::string_view text;
            std::size_t width;
            std::size_t next{0};
            WrappedLine line{};
            bool last{false};
            bool finished{false};

            // False when the line ends before the word; the next line then starts where this one stopped.
            constexpr bool addCutWord(const std::size_t wordStart, const std::size_t wordEnd)
            {
                std::size_t clusterStart = wordStart;

                for (const std::string_view cluster :
                     Text::Grapheme::Range{this->text.substr(wordStart, wordEnd - wordStart)}) {
                    const std::size_t clusterWidth = Text::measureGraphemeDisplayWidth(cluster);

                    if (this->line.width > 0 && this->line.width + clusterWidth > this->width) {
                        this->next = clusterStart;

                        return false;
                    }

                    clusterStart += cluster.size();
                    this->line.end = clusterStart;
                    this->line.width += clusterWidth;
                }

                return true;
            }

            // False when the line ends before the word; the next line then starts where this one stopped.
            constexpr bool addWord(const std::size_t spacesStart,
                                   const std::size_t wordStart,
                                   const std::size_t wordEnd)
            {
                const std::size_t spaces = wordStart - spacesStart;
                const std::size_t wordWidth = Text::measureDisplayWidth(
                    this->text.substr(wordStart, wordEnd - wordStart));

                if (this->line.width > 0 && this->line.width + spaces + wordWidth > this->width) {
                    this->next = wordStart;

                    return false;
                }

                this->line.end = wordStart;
                this->line.width += spaces;

                if (this->line.width > 0 && this->line.width + wordWidth > this->width) {
                    this->next = wordStart;

                    return false;
                }

                if (wordWidth > this->width) {
                    return this->addCutWord(wordStart, wordEnd);
                }

                this->line.end = wordEnd;
                this->line.width += wordWidth;

                return true;
            }

            constexpr void advance()
            {
                if (this->last) {
                    this->finished = true;

                    return;
                }

                const std::size_t lineBreak = std::min(this->text.find('\n', this->next), this->text.size());
                std::size_t index = this->next;

                this->line = WrappedLine{.start = this->next, .end = this->next, .width = 0};

                while (index < lineBreak) {
                    const std::size_t wordStart = std::min(this->text.find_first_not_of(' ', index), lineBreak);
                    const std::size_t wordEnd = std::min(this->text.find(' ', wordStart), lineBreak);

                    if (wordStart == lineBreak) {
                        if (this->line.width + (lineBreak - index) <= this->width) {
                            this->line.end = lineBreak;
                            this->line.width += lineBreak - index;
                        }

                        break;
                    }

                    if (!this->addWord(index, wordStart, wordEnd)) {
                        return;
                    }

                    index = wordEnd;
                }

                this->next = lineBreak + 1;
                this->last = lineBreak == this->text.size();
            }

        public:
            // Named as std::iterator_traits requires.
            using value_type = WrappedLine;         // NOLINT(readability-identifier-naming)
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            // The text must be valid UTF-8 without escape sequences. Throws std::invalid_argument for a width of 0.
            constexpr WrapIterator(const std::string_view text, const std::size_t width)
                : text{text},
                  width{width}
            {
                if (width == 0) {
                    throw std::invalid_argument{"The width must be greater than zero"};
                }

                this->advance();
            }

            [[nodiscard]]
            constexpr const WrappedLine& operator*() const
            {
                return this->line;
            }

            constexpr WrapIterator& operator++()
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
} // namespace Consolpp::Text
