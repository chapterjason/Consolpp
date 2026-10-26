#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/OptionPresence.hpp"
#include "Consolpp/Parsing/OptionState.hpp"
#include "Consolpp/Parsing/OptionalValue.hpp"
#include "Consolpp/Parsing/ParseError.hpp"
#include "Consolpp/Parsing/ParseErrorKind.hpp"
#include "Consolpp/Parsing/RepeatedValueRange.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Parsing/VariadicState.hpp"
#include "Consolpp/Parsing/VariadicValueRange.hpp"
#include "Consolpp/Text/FixedString.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <deque>
#include <iterator>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace Consolpp::Parsing
{
    template <const auto& CommandDefinition>
    class Parameters
    {
        private:
            static constexpr std::size_t optionCount = CommandDefinition.signature.getOptions().size();
            static constexpr std::size_t argumentCount = CommandDefinition.signature.getArguments().size();

            Parsing::TokenRange tokens;
            std::array<OptionState, optionCount> options;
            std::array<std::string_view, argumentCount> arguments;
            std::size_t givenArguments;
            VariadicState variadic;
            // Values set after parsing; a deque keeps them in place, so the views on them stay valid.
            std::deque<std::string> setValues;
            std::array<bool, argumentCount> setArguments{};
            std::vector<std::string_view> setVariadicValues;

            [[nodiscard]]
            bool isAbsent(const std::size_t index) const
            {
                return index >= this->givenArguments && !this->setArguments.at(index);
            }

            template <typename Declaration, std::size_t Count>
            static consteval std::size_t findDeclaration(const std::array<Declaration, Count>& declared,
                                                         const std::string_view name,
                                                         const char* const missing)
            {
                const auto found = std::ranges::find(declared, name, &Declaration::name);

                if (found == declared.end()) {
                    throw std::invalid_argument{missing};
                }

                return static_cast<std::size_t>(std::distance(declared.begin(), found));
            }

            template <Text::FixedString Name>
            static consteval std::size_t findOption()
            {
                return Parameters::findDeclaration(CommandDefinition.signature.getOptions(),
                                                   Name.getView(),
                                                   "The signature has no option with this name");
            }

            template <Text::FixedString Name>
            static consteval std::size_t findArgument()
            {
                return Parameters::findDeclaration(CommandDefinition.signature.getArguments(),
                                                   Name.getView(),
                                                   "The signature has no argument with this name");
            }

            template <std::size_t Index>
            [[nodiscard]]
            auto getRepeatedOption() const
            {
                constexpr Option option = std::get<Index>(CommandDefinition.signature.getOptions());
                const OptionState& state = std::get<Index>(this->options);

                using Value = std::conditional_t<option.value == OptionValue::Required,
                                                 std::string_view,
                                                 std::optional<std::string_view>>;

                const Parsing::TokenRange remaining{this->tokens.getArguments().subspan(state.firstPosition)};
                const Parsing::TokenIterator first = state.presence == OptionPresence::Absent ? remaining.end()
                                                                                              : remaining.begin();

                const std::span<const std::string_view> defaults = state.presence == OptionPresence::Absent
                                                                       ? option.defaultValues
                                                                       : std::span<const std::string_view>{};

                return RepeatedValueRange<Value>{
                    CommandDefinition.signature.getOptions(),
                    std::get<Index>(CommandDefinition.signature.getOptions()),
                    first,
                    remaining.end(),
                    defaults,
                };
            }

            template <std::size_t Index>
            [[nodiscard]]
            auto getRequiredValueOption() const
            {
                constexpr Option option = std::get<Index>(CommandDefinition.signature.getOptions());
                const OptionState& state = std::get<Index>(this->options);

                if constexpr (option.defaultValue.has_value()) {
                    constexpr std::string_view fallback = *option.defaultValue;

                    if (state.presence == OptionPresence::Absent) {
                        return fallback;
                    }

                    return state.value;
                } else {
                    if (state.presence == OptionPresence::Absent) {
                        return std::optional<std::string_view>{};
                    }

                    return std::optional<std::string_view>{state.value};
                }
            }

            template <std::size_t Index>
            [[nodiscard]]
            auto getOptionalValueOption() const
            {
                constexpr Option option = std::get<Index>(CommandDefinition.signature.getOptions());
                const OptionState& state = std::get<Index>(this->options);

                if constexpr (option.defaultValue.has_value()) {
                    constexpr std::string_view fallback = *option.defaultValue;

                    if (state.presence == OptionPresence::Absent) {
                        return std::optional<std::string_view>{fallback};
                    }

                    if (!state.valued) {
                        return std::optional<std::string_view>{};
                    }

                    return std::optional<std::string_view>{state.value};
                } else {
                    if (state.presence == OptionPresence::Absent) {
                        return OptionalValue{};
                    }

                    if (!state.valued) {
                        return OptionalValue{.given = true};
                    }

                    return OptionalValue{.given = true, .value = state.value};
                }
            }

            template <std::size_t Index>
            [[nodiscard]]
            auto getFlagOption() const
            {
                constexpr Option option = std::get<Index>(CommandDefinition.signature.getOptions());
                const OptionState& state = std::get<Index>(this->options);

                if constexpr (option.defaultEnabled.has_value()) {
                    constexpr bool fallback = *option.defaultEnabled;

                    if (state.presence == OptionPresence::Absent) {
                        return fallback;
                    }

                    return state.presence == OptionPresence::Given;
                } else {
                    if (state.presence == OptionPresence::Absent) {
                        return std::optional<bool>{};
                    }

                    return std::optional<bool>{state.presence == OptionPresence::Given};
                }
            }

            static constexpr std::size_t variadicIndex()
            {
                const auto& declared = CommandDefinition.signature.getArguments();
                const auto found = std::ranges::find(declared, true, &Argument::variadic);

                return static_cast<std::size_t>(std::distance(declared.begin(), found));
            }

        public:
            Parameters(const Parsing::TokenRange tokens,
                       const std::array<OptionState, optionCount>& options,
                       const std::array<std::string_view, argumentCount>& arguments,
                       const std::size_t givenArguments,
                       const VariadicState variadic)
                : tokens{tokens},
                  options{options},
                  arguments{arguments},
                  givenArguments{givenArguments},
                  variadic{variadic}
            {
            }

            // The views on the set values would point into the copy's source.
            Parameters(const Parameters&) = delete;
            Parameters& operator=(const Parameters&) = delete;
            Parameters(Parameters&&) = default;
            Parameters& operator=(Parameters&&) = default;
            ~Parameters() = default;

            // A required argument that was neither given nor set.
            [[nodiscard]]
            bool isArgumentMissing(const std::size_t index) const
            {
                return CommandDefinition.signature.getArguments().at(index).required && this->isAbsent(index);
            }

            // Throws ParseError for the first missing argument.
            void requireArguments() const
            {
                for (std::size_t index = 0; index < argumentCount; ++index) {
                    if (this->isArgumentMissing(index)) {
                        throw ParseError{
                            ParseErrorKind::MissingArgument,
                            CommandDefinition.signature.getArguments().at(index).name,
                            0,
                        };
                    }
                }
            }

            // For an argument that is not variadic; the value is copied.
            void setArgument(const std::size_t index, const std::string_view value)
            {
                if (index == Parameters::variadicIndex()) {
                    throw std::invalid_argument{"A variadic argument takes several values"};
                }

                this->arguments.at(index) = this->setValues.emplace_back(value);
                this->setArguments.at(index) = true;
            }

            // For the variadic argument; the values are copied.
            void setArgument(const std::size_t index, const std::span<const std::string_view> values)
            {
                if (index != Parameters::variadicIndex()) {
                    throw std::invalid_argument{"Only the variadic argument takes several values"};
                }

                this->setVariadicValues.clear();

                for (const std::string_view value : values) {
                    this->setVariadicValues.emplace_back(this->setValues.emplace_back(value));
                }

                this->setArguments.at(index) = true;
            }

            template <Text::FixedString Name>
            void setArgument(const std::string_view value)
            {
                this->setArgument(Parameters::findArgument<Name>(), value);
            }

            template <Text::FixedString Name>
            void setArgument(const std::span<const std::string_view> values)
            {
                this->setArgument(Parameters::findArgument<Name>(), values);
            }

            template <Text::FixedString Name>
            [[nodiscard]]
            auto getOption() const
            {
                constexpr std::size_t index = Parameters::findOption<Name>();
                constexpr Option option = std::get<index>(CommandDefinition.signature.getOptions());

                if constexpr (option.repeatable) {
                    return this->getRepeatedOption<index>();
                } else if constexpr (option.value == OptionValue::Required) {
                    return this->getRequiredValueOption<index>();
                } else if constexpr (option.value == OptionValue::Optional) {
                    return this->getOptionalValueOption<index>();
                } else {
                    return this->getFlagOption<index>();
                }
            }

            template <Text::FixedString Name>
            [[nodiscard]]
            auto getArgument() const
            {
                constexpr std::size_t index = Parameters::findArgument<Name>();
                constexpr Argument argument = std::get<index>(CommandDefinition.signature.getArguments());

                if constexpr (argument.variadic) {
                    const Parsing::TokenRange remaining{
                        this->tokens.getArguments().subspan(this->variadic.firstPosition)};
                    const bool given = index < this->givenArguments;
                    const Parsing::TokenIterator first = given ? remaining.begin() : remaining.end();
                    std::span<const std::string_view> defaults = argument.defaultValues;

                    if (given) {
                        defaults = std::span<const std::string_view>{};
                    } else if (this->setArguments.at(index)) {
                        defaults = this->setVariadicValues;
                    }

                    return VariadicValueRange{
                        CommandDefinition.signature.getOptions(),
                        first,
                        remaining.end(),
                        !this->variadic.afterSeparator,
                        defaults,
                    };
                } else if constexpr (argument.required) {
                    return std::get<index>(this->arguments);
                } else if constexpr (argument.defaultValue.has_value()) {
                    constexpr std::string_view fallback = *argument.defaultValue;

                    if (this->isAbsent(index)) {
                        return fallback;
                    }

                    return std::get<index>(this->arguments);
                } else {
                    if (this->isAbsent(index)) {
                        return std::optional<std::string_view>{};
                    }

                    return std::optional<std::string_view>{std::get<index>(this->arguments)};
                }
            }
    };
} // namespace Consolpp::Parsing
