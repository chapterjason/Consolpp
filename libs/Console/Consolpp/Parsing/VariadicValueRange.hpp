#pragma once

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"
#include "Consolpp/Parsing/VariadicValueIterator.hpp"

#include <iterator>
#include <span>

namespace Consolpp::Parsing
{
    class VariadicValueRange
    {
        private:
            std::span<const Option> options;
            Parsing::TokenIterator first;
            Parsing::TokenIterator last;
            std::span<const std::string_view> defaults;
            bool parseOptions;

        public:
            // Non-empty defaults replace the values from argv, so they are only passed for an absent argument.
            VariadicValueRange(const std::span<const Option> options,
                               const Parsing::TokenIterator first,
                               const Parsing::TokenIterator last,
                               const bool parseOptions,
                               const std::span<const std::string_view> defaults)
                : options{options},
                  first{first},
                  last{last},
                  defaults{defaults},
                  parseOptions{parseOptions}
            {
            }

            [[nodiscard]]
            VariadicValueIterator begin() const
            {
                if (!this->defaults.empty()) {
                    return VariadicValueIterator{this->defaults};
                }

                return VariadicValueIterator{this->options, this->first, this->last, this->parseOptions};
            }

            [[nodiscard]]
            static std::default_sentinel_t end()
            {
                return std::default_sentinel;
            }
    };
} // namespace Consolpp::Parsing
