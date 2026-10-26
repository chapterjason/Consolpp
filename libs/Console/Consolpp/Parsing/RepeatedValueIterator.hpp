#pragma once

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/HasValueInNextArgument.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Parsing
{
    template <typename Value>
        requires std::same_as<Value, std::string_view> || std::same_as<Value, std::optional<std::string_view>>
    class RepeatedValueIterator
    {
        private:
            std::span<const Option> options;
            std::span<const std::string_view> defaults;
            const Option* option;
            Parsing::TokenIterator next;
            Parsing::TokenIterator end;
            Value value;
            std::size_t defaultIndex{0};
            bool exhausted{false};
            bool fromDefaults{false};

            void advanceDefaults()
            {
                if (this->defaultIndex == this->defaults.size()) {
                    this->exhausted = true;

                    return;
                }

                this->value = Value{
                    *std::next(this->defaults.begin(), static_cast<std::ptrdiff_t>(this->defaultIndex)),
                };
                ++this->defaultIndex;
            }

            void setValue(std::optional<std::string_view> found)
            {
                if (!found.has_value() && this->option->value == OptionValue::Required && this->next != this->end) {
                    found = (*this->next).getArgument();
                    ++this->next;
                }

                if constexpr (std::same_as<Value, std::string_view>) {
                    if (!found.has_value()) {
                        throw std::logic_error{"The required option has no value"};
                    }

                    this->value = *found;
                } else {
                    this->value = found;
                }
            }

            bool consumeShortOptions(const Parsing::Token& token)
            {
                const std::string_view shortcuts = token.getName();

                for (std::size_t position = 0; position < shortcuts.size(); ++position) {
                    const char shortcut = shortcuts.at(position);

                    if (shortcut == this->option->shortcut) {
                        const std::string_view rest = shortcuts.substr(position + 1);

                        this->setValue(rest.empty() ? std::nullopt : std::optional<std::string_view>{rest});

                        return true;
                    }

                    const auto found = std::ranges::find(this->options, shortcut, &Option::shortcut);

                    if (found != this->options.end() && found->value != OptionValue::None) {
                        return false;
                    }
                }

                return false;
            }

            bool consume(const Parsing::Token& token)
            {
                if (token.getKind() == Parsing::TokenKind::ShortOptions) {
                    return this->consumeShortOptions(token);
                }

                if (token.getKind() != Parsing::TokenKind::LongOption || token.getName() != this->option->name) {
                    return false;
                }

                this->setValue(token.getValue());

                return true;
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

                    if (token.getKind() == Parsing::TokenKind::Separator) {
                        break;
                    }

                    if (this->consume(token)) {
                        return;
                    }

                    if (Parsing::hasValueInNextArgument(this->options, token) && this->next != this->end) {
                        ++this->next;
                    }
                }

                this->exhausted = true;
            }

        public:
            // Named as std::iterator_traits requires.
            using value_type = Value;               // NOLINT(readability-identifier-naming)
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            RepeatedValueIterator(const std::span<const Option> options,
                                  const Option& option,
                                  const Parsing::TokenIterator first,
                                  const Parsing::TokenIterator end)
                : options{options},
                  option{&option},
                  next{first},
                  end{end}
            {
                this->advance();
            }

            explicit RepeatedValueIterator(const std::span<const std::string_view> defaults)
                : defaults{defaults},
                  option{nullptr},
                  next{std::span<const char* const>::iterator{}},
                  end{std::span<const char* const>::iterator{}},
                  fromDefaults{true}
            {
                this->advance();
            }

            [[nodiscard]]
            const Value& operator*() const
            {
                return this->value;
            }

            RepeatedValueIterator& operator++()
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
