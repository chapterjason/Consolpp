#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/OptionPresence.hpp"
#include "Consolpp/Parsing/OptionState.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/ParseError.hpp"
#include "Consolpp/Parsing/ParseErrorKind.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Parsing/VariadicState.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <span>
#include <string_view>

namespace Consolpp::Parsing
{
    template <const auto& CommandDefinition>
    class Parser
    {
        private:
            static constexpr std::size_t optionCount = CommandDefinition.signature.getOptions().size();
            static constexpr std::size_t argumentCount = CommandDefinition.signature.getArguments().size();
            static constexpr std::uint8_t shortDashes = 1;
            static constexpr std::uint8_t longDashes = 2;
            static constexpr bool hasVariadicArgument = std::ranges::any_of(CommandDefinition.signature.getArguments(),
                                                                            &Argument::variadic);

            Parsing::TokenRange tokens;
            std::array<OptionState, optionCount> options;
            std::array<std::string_view, argumentCount> arguments;
            std::size_t givenArguments{0};
            VariadicState variadic;
            std::uint32_t position{0};
            bool afterSeparator{false};

            // Compact copies of the names and shortcuts; measured faster in the parser than walking the options.
            static constexpr std::array<std::string_view, optionCount> optionNames = [] {
                std::array<std::string_view, Parser::optionCount> names{};

                for (std::size_t index = 0; index < Parser::optionCount; ++index) {
                    names.at(index) = CommandDefinition.signature.getOptions().at(index).name;
                }

                return names;
            }();
            static constexpr std::array<std::optional<char>, optionCount> optionShortcuts = [] {
                std::array<std::optional<char>, Parser::optionCount> shortcuts{};

                for (std::size_t index = 0; index < Parser::optionCount; ++index) {
                    shortcuts.at(index) = CommandDefinition.signature.getOptions().at(index).shortcut;
                }

                return shortcuts;
            }();

            // The option at the place of the key in one of the compact copies, nullptr when the key is not in it.
            template <typename Value, typename Key>
            static const Option* findOption(const std::array<Value, optionCount>& column, const Key key)
            {
                const auto found = std::ranges::find(column, key);

                if (found == column.end()) {
                    return nullptr;
                }

                return &CommandDefinition.signature.getOptions().at(
                    static_cast<std::size_t>(std::distance(column.begin(), found)));
            }

            static std::size_t getIndex(const Option& option)
            {
                return static_cast<std::size_t>(&option - CommandDefinition.signature.getOptions().data());
            }

            std::optional<std::string_view> takeNextValue()
            {
                const std::span<const char* const> remaining = this->tokens.getArguments().subspan(this->position + 1);

                if (remaining.empty()) {
                    return std::nullopt;
                }

                ++this->position;

                return std::string_view{remaining.front()};
            }

            void setOption(const Option& option,
                           std::optional<std::string_view> value,
                           const std::string_view subject,
                           const std::uint8_t dashes)
            {
                OptionState& state = this->options.at(Parser::getIndex(option));
                const std::uint32_t occurrence = this->position;

                if (state.presence != OptionPresence::Absent && !option.repeatable) {
                    throw ParseError{ParseErrorKind::DuplicateOption, subject, dashes};
                }

                if (option.value == OptionValue::None && value.has_value()) {
                    throw ParseError{ParseErrorKind::UnexpectedValue, subject, dashes};
                }

                if (option.value == OptionValue::Required && !value.has_value()) {
                    value = this->takeNextValue();
                }

                if (option.value == OptionValue::Required && !value.has_value()) {
                    throw ParseError{ParseErrorKind::MissingValue, subject, dashes};
                }

                if (state.presence != OptionPresence::Absent) {
                    return;
                }

                state = OptionState{
                    .value = value.value_or(std::string_view{}),
                    .presence = OptionPresence::Given,
                    .valued = value.has_value(),
                    .firstPosition = occurrence,
                };
            }

            void setNegationOption(const std::string_view name, const std::optional<std::string_view>& value)
            {
                constexpr std::string_view prefix{"no-"};

                if (!name.starts_with(prefix)) {
                    throw ParseError{ParseErrorKind::UnknownOption, name, Parser::longDashes};
                }

                const Option* const found = Parser::findOption(Parser::optionNames, name.substr(prefix.size()));

                if (found == nullptr || found->value != OptionValue::None) {
                    throw ParseError{ParseErrorKind::UnknownOption, name, Parser::longDashes};
                }

                OptionState& state = this->options.at(Parser::getIndex(*found));

                if (state.presence != OptionPresence::Absent) {
                    throw ParseError{ParseErrorKind::DuplicateOption, name, Parser::longDashes};
                }

                if (value.has_value()) {
                    throw ParseError{ParseErrorKind::UnexpectedValue, name, Parser::longDashes};
                }

                state = OptionState{.value = {}, .presence = OptionPresence::Negated, .valued = false};
            }

            void parseLongOption(const Parsing::Token& token)
            {
                const std::string_view name = token.getName();
                const Option* const found = Parser::findOption(Parser::optionNames, name);

                if (found == nullptr) {
                    this->setNegationOption(name, token.getValue());

                    return;
                }

                this->setOption(*found, token.getValue(), name, Parser::longDashes);
            }

            void parseShortOptions(const Parsing::Token& token)
            {
                const std::string_view shortcuts = token.getName();

                for (std::size_t position = 0; position < shortcuts.size(); ++position) {
                    const std::string_view subject = shortcuts.substr(position, 1);
                    const Option* const found = Parser::findOption(Parser::optionShortcuts, subject.front());

                    if (found == nullptr) {
                        throw ParseError{ParseErrorKind::UnknownOption, subject, Parser::shortDashes};
                    }

                    if (found->value == OptionValue::None) {
                        this->setOption(*found, std::nullopt, subject, Parser::shortDashes);

                        continue;
                    }

                    const std::string_view rest = shortcuts.substr(position + 1);

                    this->setOption(*found,
                                    rest.empty() ? std::nullopt : std::optional<std::string_view>{rest},
                                    subject,
                                    Parser::shortDashes);

                    return;
                }
            }

            void addArgument(const std::string_view argument)
            {
                if (this->givenArguments == Parser::argumentCount && Parser::hasVariadicArgument) {
                    return;
                }

                if (this->givenArguments == Parser::argumentCount) {
                    throw ParseError{ParseErrorKind::TooManyArguments, argument, 0};
                }

                if (Parser::hasVariadicArgument && this->givenArguments + 1 == Parser::argumentCount) {
                    this->variadic = VariadicState{
                        .firstPosition = this->position,
                        .afterSeparator = this->afterSeparator,
                    };
                }

                this->arguments.at(this->givenArguments) = argument;
                ++this->givenArguments;
            }

        public:
            explicit Parser(const Parsing::TokenRange tokens)
                : tokens{tokens}
            {
            }

            [[nodiscard]]
            Parameters<CommandDefinition> parse()
            {
                const std::span<const char* const> arguments = this->tokens.getArguments();

                for (; this->position < arguments.size(); ++this->position) {
                    const Parsing::Token token = *Parsing::TokenIterator{std::next(arguments.begin(), this->position)};

                    if (this->afterSeparator) {
                        this->addArgument(token.getArgument());

                        continue;
                    }

                    switch (token.getKind()) {
                        case Parsing::TokenKind::LongOption:
                            this->parseLongOption(token);
                            break;
                        case Parsing::TokenKind::ShortOptions:
                            this->parseShortOptions(token);
                            break;
                        case Parsing::TokenKind::Separator:
                            this->afterSeparator = true;
                            break;
                        case Parsing::TokenKind::Dash:
                        case Parsing::TokenKind::Positional:
                            this->addArgument(token.getArgument());
                            break;
                    }
                }

                return Parameters<CommandDefinition>{
                    this->tokens,
                    this->options,
                    this->arguments,
                    this->givenArguments,
                    this->variadic,
                };
            }
    };
} // namespace Consolpp::Parsing
