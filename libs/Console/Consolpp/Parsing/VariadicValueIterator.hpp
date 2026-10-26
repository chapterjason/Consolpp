#pragma once

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/HasValueInNextArgument.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"

#include <cstddef>
#include <iterator>
#include <span>
#include <string_view>

namespace Consolpp::Parsing
{
    class VariadicValueIterator
    {
        private:
            std::span<const Option> options;
            std::span<const std::string_view> defaults;
            Parsing::TokenIterator next;
            Parsing::TokenIterator end;
            std::string_view value;
            std::size_t defaultIndex{0};
            bool parseOptions;
            bool exhausted{false};
            bool fromDefaults{false};

            void advanceDefaults()
            {
                if (this->defaultIndex == this->defaults.size()) {
                    this->exhausted = true;

                    return;
                }

                this->value = *std::next(this->defaults.begin(), static_cast<std::ptrdiff_t>(this->defaultIndex));
                ++this->defaultIndex;
            }

            void advance()
            {
                if (this->fromDefaults) {
                    this->advanceDefaults();

                    return;
                }

                while (this->next != this->end) {
                    const Parsing::Token token = *this->next;

                    ++this->next;

                    if (!this->parseOptions
                        || token.getKind() == Parsing::TokenKind::Positional
                        || token.getKind() == Parsing::TokenKind::Dash) {
                        this->value = token.getArgument();

                        return;
                    }

                    if (token.getKind() == Parsing::TokenKind::Separator) {
                        this->parseOptions = false;

                        continue;
                    }

                    if (Parsing::hasValueInNextArgument(this->options, token) && this->next != this->end) {
                        ++this->next;
                    }
                }

                this->exhausted = true;
            }

        public:
            // Named as std::iterator_traits requires.
            using value_type = std::string_view;    // NOLINT(readability-identifier-naming)
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            VariadicValueIterator(const std::span<const Option> options,
                                  const Parsing::TokenIterator first,
                                  const Parsing::TokenIterator end,
                                  const bool parseOptions)
                : options{options},
                  next{first},
                  end{end},
                  parseOptions{parseOptions}
            {
                this->advance();
            }

            explicit VariadicValueIterator(const std::span<const std::string_view> defaults)
                : defaults{defaults},
                  next{std::span<const char* const>::iterator{}},
                  end{std::span<const char* const>::iterator{}},
                  parseOptions{false},
                  fromDefaults{true}
            {
                this->advance();
            }

            [[nodiscard]]
            const std::string_view& operator*() const
            {
                return this->value;
            }

            VariadicValueIterator& operator++()
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
} // namespace Consolpp::Parsing
