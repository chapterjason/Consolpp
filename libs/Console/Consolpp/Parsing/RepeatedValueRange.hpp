#pragma once

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/RepeatedValueIterator.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"

#include <concepts>
#include <iterator>
#include <optional>
#include <span>
#include <string_view>

namespace Consolpp::Parsing
{
    template <typename Value>
        requires std::same_as<Value, std::string_view> || std::same_as<Value, std::optional<std::string_view>>
    class RepeatedValueRange
    {
        private:
            std::span<const Option> options;
            const Option* option;
            Parsing::TokenIterator first;
            Parsing::TokenIterator last;
            std::span<const std::string_view> defaults;

        public:
            // Non-empty defaults replace the values from argv, so they are only passed for an absent option.
            RepeatedValueRange(const std::span<const Option> options,
                               const Option& option,
                               const Parsing::TokenIterator first,
                               const Parsing::TokenIterator last,
                               const std::span<const std::string_view> defaults)
                : options{options},
                  option{&option},
                  first{first},
                  last{last},
                  defaults{defaults}
            {
            }

            [[nodiscard]]
            RepeatedValueIterator<Value> begin() const
            {
                if (!this->defaults.empty()) {
                    return RepeatedValueIterator<Value>{this->defaults};
                }

                return RepeatedValueIterator<Value>{this->options, *this->option, this->first, this->last};
            }

            [[nodiscard]]
            std::default_sentinel_t end() const
            {
                return std::default_sentinel;
            }
    };
} // namespace Consolpp::Parsing
