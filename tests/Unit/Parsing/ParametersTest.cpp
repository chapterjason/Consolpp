#include "Consolpp/Parsing/Parameters.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/OptionPresence.hpp"
#include "Consolpp/Parsing/OptionState.hpp"
#include "Consolpp/Parsing/OptionalValue.hpp"
#include "Consolpp/Parsing/ParseError.hpp"
#include "Consolpp/Parsing/RepeatedValueRange.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Parsing/VariadicState.hpp"
#include "Consolpp/Parsing/VariadicValueRange.hpp"
#include "Consolpp/Signature.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::OptionalValue;
    using Consolpp::Parsing::OptionPresence;
    using Consolpp::Parsing::OptionState;
    using Consolpp::Parsing::VariadicState;

    namespace
    {
        constexpr Signature signature{
            std::array{
                Option{.name = "force"},
                Option{.name = "color"},
                Option{.name = "output", .value = OptionValue::Required},
                Option{.name = "tag", .value = OptionValue::Optional},
                Option{.name = "include", .value = OptionValue::Required, .repeatable = true},
                Option{.name = "define", .value = OptionValue::Optional, .repeatable = true},
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "target", .required = false},
            },
        };

        constexpr Definition definition{.name = "deploy", .signature = signature};

        using Parameters = Consolpp::Parsing::Parameters<definition>;

        constexpr std::size_t optionCount = signature.getOptions().size();

        constexpr std::array<OptionState, optionCount> noOptions{};
        constexpr std::array<std::string_view, 2> noArguments{};
        constexpr VariadicState noVariadic{};

        constexpr Signature variadicSignature{
            std::array{
                Option{.name = "force", .shortcut = 'f'},
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "files", .required = false, .variadic = true},
            },
        };

        constexpr Definition variadicDefinition{.name = "deploy", .signature = variadicSignature};

        using VariadicParameters = Consolpp::Parsing::Parameters<variadicDefinition>;

        constexpr Signature defaultsSignature{
            std::array{
                Option{.name = "environment", .value = OptionValue::Required, .defaultValue = "staging"},
                Option{.name = "tag", .value = OptionValue::Optional, .defaultValue = "stable"},
                Option{.name = "color", .defaultEnabled = true},
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "target", .required = false, .defaultValue = "."},
            },
        };

        constexpr Definition defaultsDefinition{.name = "deploy", .signature = defaultsSignature};

        using DefaultsParameters = Consolpp::Parsing::Parameters<defaultsDefinition>;

        constexpr std::size_t defaultsOptionCount = defaultsSignature.getOptions().size();

        constexpr std::array<OptionState, defaultsOptionCount> noDefaultsOptions{};

        constexpr std::array<std::string_view, 2> includeDefaults{"src", "lib"};
        constexpr std::array<std::string_view, 1> fileDefaults{"main.cpp"};

        constexpr Signature listDefaultsSignature{
            std::array{
                Option{
                    .name = "include",
                    .value = OptionValue::Required,
                    .repeatable = true,
                    .defaultValues = includeDefaults,
                },
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "files", .required = false, .variadic = true, .defaultValues = fileDefaults},
            },
        };

        constexpr Definition listDefaultsDefinition{.name = "deploy", .signature = listDefaultsSignature};

        using ListDefaultsParameters = Consolpp::Parsing::Parameters<listDefaultsDefinition>;

        constexpr std::array<OptionState, 1> noListDefaultsOptions{};

        constexpr std::array<OptionState, 1> noVariadicOptions{};
    } // namespace

    TEST(ParametersTest, GetOptionReturnsAnOptionalBoolForAFlag)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        using Result = decltype(parameters.getOption<"color">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, std::optional<bool>>));
    }

    TEST(ParametersTest, GetOptionReturnsAnOptionalViewForARequiredValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        using Result = decltype(parameters.getOption<"output">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, std::optional<std::string_view>>));
    }

    TEST(ParametersTest, GetOptionReturnsANestedOptionalViewForAnOptionalValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        using Result = decltype(parameters.getOption<"tag">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, OptionalValue>));
    }

    TEST(ParametersTest, GetOptionReturnsNothingForAnAbsentFlag)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        const std::optional<bool> color = parameters.getOption<"color">();

        // Assert
        EXPECT_FALSE(color.has_value());
    }

    TEST(ParametersTest, GetOptionReturnsTrueForAGivenFlag)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, optionCount> states{};

        std::get<1>(states) = OptionState{.value = {}, .presence = OptionPresence::Given};

        const Parameters parameters{tokens, states, noArguments, 0, noVariadic};
        constexpr std::optional<bool> expected{true};

        // Act
        const std::optional<bool> color = parameters.getOption<"color">();

        // Assert
        EXPECT_EQ(color, expected);
    }

    TEST(ParametersTest, GetOptionReturnsFalseForANegatedFlag)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, optionCount> states{};

        std::get<1>(states) = OptionState{.value = {}, .presence = OptionPresence::Negated};

        const Parameters parameters{tokens, states, noArguments, 0, noVariadic};
        constexpr std::optional<bool> expected{false};

        // Act
        const std::optional<bool> color = parameters.getOption<"color">();

        // Assert
        EXPECT_EQ(color, expected);
    }

    TEST(ParametersTest, GetOptionReturnsNothingForAnAbsentRequiredValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        const std::optional<std::string_view> output = parameters.getOption<"output">();

        // Assert
        EXPECT_FALSE(output.has_value());
    }

    TEST(ParametersTest, GetOptionReturnsTheGivenRequiredValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, optionCount> states{};

        std::get<2>(states) = OptionState{.value = "out.txt", .presence = OptionPresence::Given, .valued = true};

        const Parameters parameters{tokens, states, noArguments, 0, noVariadic};
        constexpr std::optional<std::string_view> expected{"out.txt"};

        // Act
        const std::optional<std::string_view> output = parameters.getOption<"output">();

        // Assert
        EXPECT_EQ(output, expected);
    }

    TEST(ParametersTest, GetOptionReturnsNothingForAnAbsentOptionalValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        const OptionalValue tag = parameters.getOption<"tag">();

        // Assert
        EXPECT_FALSE(tag.given);
    }

    TEST(ParametersTest, GetOptionReturnsNoValueForAnOptionalValueGivenWithout)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, optionCount> states{};

        std::get<3>(states) = OptionState{.value = {}, .presence = OptionPresence::Given};

        const Parameters parameters{tokens, states, noArguments, 0, noVariadic};
        constexpr OptionalValue expected{.given = true};

        // Act
        const OptionalValue tag = parameters.getOption<"tag">();

        // Assert
        EXPECT_EQ(tag, expected);
    }

    TEST(ParametersTest, GetOptionReturnsTheGivenOptionalValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, optionCount> states{};

        std::get<3>(states) = OptionState{.value = "stable", .presence = OptionPresence::Given, .valued = true};

        const Parameters parameters{tokens, states, noArguments, 0, noVariadic};
        constexpr OptionalValue expected{.given = true, .value = "stable"};

        // Act
        const OptionalValue tag = parameters.getOption<"tag">();

        // Assert
        EXPECT_EQ(tag, expected);
    }

    TEST(ParametersTest, GetOptionKeepsAnEmptyGivenOptionalValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, optionCount> states{};

        std::get<3>(states) = OptionState{.value = "", .presence = OptionPresence::Given, .valued = true};

        const Parameters parameters{tokens, states, noArguments, 0, noVariadic};
        constexpr OptionalValue expected{.given = true, .value = ""};

        // Act
        const OptionalValue tag = parameters.getOption<"tag">();

        // Assert
        EXPECT_EQ(tag, expected);
    }

    TEST(ParametersTest, GetArgumentReturnsAViewForARequiredArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, std::array<std::string_view, 2>{"a.txt", ""}, 1, noVariadic};

        // Act
        using Result = decltype(parameters.getArgument<"source">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, std::string_view>));
    }

    TEST(ParametersTest, GetArgumentReturnsAnOptionalViewForAnOptionalArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, std::array<std::string_view, 2>{"a.txt", ""}, 1, noVariadic};

        // Act
        using Result = decltype(parameters.getArgument<"target">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, std::optional<std::string_view>>));
    }

    TEST(ParametersTest, GetArgumentReturnsTheRequiredArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, std::array<std::string_view, 2>{"a.txt", ""}, 1, noVariadic};

        // Act
        const std::string_view source = parameters.getArgument<"source">();

        // Assert
        EXPECT_EQ(source, "a.txt");
    }

    TEST(ParametersTest, GetArgumentReturnsNothingForAnOptionalArgumentNotGiven)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, std::array<std::string_view, 2>{"a.txt", ""}, 1, noVariadic};

        // Act
        const std::optional<std::string_view> target = parameters.getArgument<"target">();

        // Assert
        EXPECT_FALSE(target.has_value());
    }

    TEST(ParametersTest, GetArgumentReturnsTheGivenOptionalArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens,
                                    noOptions,
                                    std::array<std::string_view, 2>{"a.txt", "b.txt"},
                                    2,
                                    noVariadic};
        constexpr std::optional<std::string_view> expected{"b.txt"};

        // Act
        const std::optional<std::string_view> target = parameters.getArgument<"target">();

        // Assert
        EXPECT_EQ(target, expected);
    }

    TEST(ParametersTest, GetArgumentKeepsAnEmptyGivenOptionalArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, std::array<std::string_view, 2>{"a.txt", ""}, 2, noVariadic};
        constexpr std::optional<std::string_view> expected{""};

        // Act
        const std::optional<std::string_view> target = parameters.getArgument<"target">();

        // Assert
        EXPECT_EQ(target, expected);
    }

    TEST(ParametersTest, GetOptionReturnsRepeatedViewsForARepeatableRequiredValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        using Result = decltype(parameters.getOption<"include">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, Consolpp::Parsing::RepeatedValueRange<std::string_view>>));
    }

    TEST(ParametersTest, GetOptionReturnsRepeatedOptionalViewsForARepeatableOptionalValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        using Result = decltype(parameters.getOption<"define">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, Consolpp::Parsing::RepeatedValueRange<std::optional<std::string_view>>>));
    }

    TEST(ParametersTest, GetOptionReturnsNoValuesForAnAbsentRepeatableOption)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--include=a"};
        const Consolpp::Parsing::TokenRange tokens{arguments};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : parameters.getOption<"include">()) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_TRUE(visited.empty());
    }

    TEST(ParametersTest, GetOptionReadsTheValuesOfARepeatableOptionFromItsFirstPosition)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"a.txt", "--include=a", "--include=b"};
        const Consolpp::Parsing::TokenRange tokens{arguments};
        std::array<OptionState, optionCount> states{};

        std::get<4>(
            states) = OptionState{.value = "a", .presence = OptionPresence::Given, .valued = true, .firstPosition = 1};

        const Parameters parameters{tokens, states, std::array<std::string_view, 2>{"a.txt", ""}, 1, noVariadic};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : parameters.getOption<"include">()) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(ParametersTest, GetArgumentReturnsVariadicValuesForAVariadicArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const VariadicParameters parameters{tokens, noVariadicOptions, noArguments, 0, noVariadic};

        // Act
        using Result = decltype(parameters.getArgument<"files">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, Consolpp::Parsing::VariadicValueRange>));
    }

    TEST(ParametersTest, GetArgumentReturnsNoValuesForAVariadicArgumentNotGiven)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"a.txt"};
        const Consolpp::Parsing::TokenRange tokens{arguments};
        const VariadicParameters parameters{tokens,
                                            noVariadicOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : parameters.getArgument<"files">()) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_TRUE(visited.empty());
    }

    TEST(ParametersTest, GetArgumentReadsTheValuesOfAVariadicArgumentFromItsFirstPosition)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"a.txt", "b.txt", "-f", "c.txt"};
        const Consolpp::Parsing::TokenRange tokens{arguments};
        constexpr VariadicState variadic{.firstPosition = 1, .afterSeparator = false};
        const VariadicParameters parameters{tokens,
                                            noVariadicOptions,
                                            std::array<std::string_view, 2>{"a.txt", "b.txt"},
                                            2,
                                            variadic};
        const std::vector<std::string_view> expected{"b.txt", "c.txt"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : parameters.getArgument<"files">()) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(ParametersTest, GetArgumentReadsOptionsAsVariadicValuesAfterTheSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"a.txt", "--", "--force"};
        const Consolpp::Parsing::TokenRange tokens{arguments};
        constexpr VariadicState variadic{.firstPosition = 2, .afterSeparator = true};
        const VariadicParameters parameters{tokens,
                                            noVariadicOptions,
                                            std::array<std::string_view, 2>{"a.txt", "--force"},
                                            2,
                                            variadic};
        const std::vector<std::string_view> expected{"--force"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : parameters.getArgument<"files">()) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(ParametersTest, GetOptionReturnsAViewForARequiredValueWithDefault)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        using Result = decltype(parameters.getOption<"environment">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, std::string_view>));
    }

    TEST(ParametersTest, GetOptionReturnsTheDefaultOfAnAbsentRequiredValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        const std::string_view environment = parameters.getOption<"environment">();

        // Assert
        EXPECT_EQ(environment, "staging");
    }

    TEST(ParametersTest, GetOptionReturnsTheGivenRequiredValueOverItsDefault)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, defaultsOptionCount> states{};

        std::get<0>(states) = OptionState{.value = "production", .presence = OptionPresence::Given, .valued = true};

        const DefaultsParameters parameters{tokens,
                                            states,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        const std::string_view environment = parameters.getOption<"environment">();

        // Assert
        EXPECT_EQ(environment, "production");
    }

    TEST(ParametersTest, GetOptionReturnsAnOptionalViewForAnOptionalValueWithDefault)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        using Result = decltype(parameters.getOption<"tag">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, std::optional<std::string_view>>));
    }

    TEST(ParametersTest, GetOptionReturnsTheDefaultOfAnAbsentOptionalValue)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};
        constexpr std::optional<std::string_view> expected{"stable"};

        // Act
        const std::optional<std::string_view> tag = parameters.getOption<"tag">();

        // Assert
        EXPECT_EQ(tag, expected);
    }

    TEST(ParametersTest, GetOptionReturnsNothingForAnOptionalValueWithDefaultGivenWithout)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, defaultsOptionCount> states{};

        std::get<1>(states) = OptionState{.value = {}, .presence = OptionPresence::Given};

        const DefaultsParameters parameters{tokens,
                                            states,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        const std::optional<std::string_view> tag = parameters.getOption<"tag">();

        // Assert
        EXPECT_FALSE(tag.has_value());
    }

    TEST(ParametersTest, GetOptionReturnsBoolForAFlagWithDefault)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        using Result = decltype(parameters.getOption<"color">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, bool>));
    }

    TEST(ParametersTest, GetOptionReturnsTheDefaultOfAnAbsentFlag)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        const bool color = parameters.getOption<"color">();

        // Assert
        EXPECT_TRUE(color);
    }

    TEST(ParametersTest, GetOptionReturnsFalseForANegatedFlagWithDefault)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        std::array<OptionState, defaultsOptionCount> states{};

        std::get<2>(states) = OptionState{.value = {}, .presence = OptionPresence::Negated};

        const DefaultsParameters parameters{tokens,
                                            states,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        const bool color = parameters.getOption<"color">();

        // Assert
        EXPECT_FALSE(color);
    }

    TEST(ParametersTest, GetArgumentReturnsAViewForAnOptionalArgumentWithDefault)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        using Result = decltype(parameters.getArgument<"target">());

        // Assert
        EXPECT_TRUE((std::is_same_v<Result, std::string_view>));
    }

    TEST(ParametersTest, GetArgumentReturnsTheDefaultOfAnOptionalArgumentNotGiven)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", ""},
                                            1,
                                            noVariadic};

        // Act
        const std::string_view target = parameters.getArgument<"target">();

        // Assert
        EXPECT_EQ(target, ".");
    }

    TEST(ParametersTest, GetArgumentReturnsTheGivenOptionalArgumentOverItsDefault)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const DefaultsParameters parameters{tokens,
                                            noDefaultsOptions,
                                            std::array<std::string_view, 2>{"a.txt", "b.txt"},
                                            2,
                                            noVariadic};

        // Act
        const std::string_view target = parameters.getArgument<"target">();

        // Assert
        EXPECT_EQ(target, "b.txt");
    }

    TEST(ParametersTest, GetOptionReadsTheDefaultsOfAnAbsentRepeatableOption)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const ListDefaultsParameters parameters{tokens,
                                                noListDefaultsOptions,
                                                std::array<std::string_view, 2>{"a.txt", ""},
                                                1,
                                                noVariadic};
        const std::vector<std::string_view> expected{"src", "lib"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : parameters.getOption<"include">()) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(ParametersTest, GetOptionReadsOnlyTheGivenValuesOfARepeatableOptionWithDefaults)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "--include=docs"};
        const Consolpp::Parsing::TokenRange tokens{arguments};
        const std::array<OptionState, 1> states{
            OptionState{.value = "docs", .presence = OptionPresence::Given, .valued = true, .firstPosition = 1},
        };
        const ListDefaultsParameters parameters{tokens,
                                                states,
                                                std::array<std::string_view, 2>{"a.txt", ""},
                                                1,
                                                noVariadic};
        const std::vector<std::string_view> expected{"docs"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : parameters.getOption<"include">()) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(ParametersTest, GetArgumentReadsTheDefaultsOfAVariadicArgumentNotGiven)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const ListDefaultsParameters parameters{tokens,
                                                noListDefaultsOptions,
                                                std::array<std::string_view, 2>{"a.txt", ""},
                                                1,
                                                noVariadic};
        const std::vector<std::string_view> expected{"main.cpp"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : parameters.getArgument<"files">()) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(ParametersTest, IsArgumentMissingForARequiredArgumentNotGiven)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        const bool missing = parameters.isArgumentMissing(0);

        // Assert
        EXPECT_TRUE(missing);
        EXPECT_FALSE(parameters.isArgumentMissing(1));
    }

    TEST(ParametersTest, SetArgumentFillsAMissingArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        parameters.setArgument<"source">("a.txt");

        // Assert
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
        EXPECT_FALSE(parameters.isArgumentMissing(0));
    }

    TEST(ParametersTest, SetArgumentFillsTheVariadicArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        VariadicParameters parameters{tokens, noVariadicOptions, noArguments, 0, noVariadic};
        const std::array<std::string_view, 2> files{"a.txt", "b.txt"};

        // Act
        parameters.setArgument<"files">(std::span<const std::string_view>{files});

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getArgument<"files">(), files));
    }

    TEST(ParametersTest, SetArgumentThrowsForSeveralValuesOfAnArgumentThatIsNotVariadic)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};
        const std::array<std::string_view, 2> values{"a.txt", "b.txt"};

        // Act
        const auto set = [&parameters, &values] {
            parameters.setArgument<"source">(std::span<const std::string_view>{values});
        };

        // Assert
        EXPECT_THROW(set(), std::invalid_argument);
    }

    TEST(ParametersTest, RequireArgumentsThrowsForAMissingArgument)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};
        const Parameters parameters{tokens, noOptions, noArguments, 0, noVariadic};

        // Act
        const auto require = [&parameters] {
            parameters.requireArguments();
        };

        // Assert
        EXPECT_THROW(require(), Consolpp::Parsing::ParseError);
    }
} // namespace Consolpp::Tests::Unit::Parsing
