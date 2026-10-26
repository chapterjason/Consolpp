#include "Consolpp/Parsing/Parser.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/OptionalValue.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/ParseError.hpp"
#include "Consolpp/Parsing/ParseErrorKind.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Signature.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::OptionalValue;
    using Consolpp::Parsing::ParseError;
    using Consolpp::Parsing::ParseErrorKind;
    using Consolpp::Parsing::TokenRange;

    namespace
    {
        constexpr Signature signature{
            std::array{
                Option{.name = "force", .shortcut = 'f'},
                Option{.name = "all", .shortcut = 'a'},
                Option{.name = "color"},
                Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
                Option{.name = "tag", .shortcut = 't', .value = OptionValue::Optional},
                Option{.name = "include", .shortcut = 'i', .value = OptionValue::Required, .repeatable = true},
                Option{.name = "define", .shortcut = 'd', .value = OptionValue::Optional, .repeatable = true},
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "target", .required = false},
            },
        };

        constexpr Definition definition{.name = "deploy", .signature = signature};

        using Parser = Consolpp::Parsing::Parser<definition>;
        using Parameters = Consolpp::Parsing::Parameters<definition>;
        using CaughtError = std::optional<std::tuple<ParseErrorKind, std::string, std::uint8_t>>;

        constexpr Signature requiredVariadicSignature{
            std::array{
                Option{.name = "force", .shortcut = 'f'},
                Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "files", .variadic = true},
            },
        };

        constexpr Definition requiredVariadicDefinition{
            .name = "deploy",
            .signature = requiredVariadicSignature,
        };

        using RequiredVariadicParser = Consolpp::Parsing::Parser<requiredVariadicDefinition>;
        using RequiredVariadicParameters = Consolpp::Parsing::Parameters<requiredVariadicDefinition>;

        constexpr Signature optionalVariadicSignature{
            std::array{
                Option{.name = "force", .shortcut = 'f'},
            },
            std::array{
                Argument{.name = "source"},
                Argument{.name = "files", .required = false, .variadic = true},
            },
        };

        constexpr Definition optionalVariadicDefinition{
            .name = "deploy",
            .signature = optionalVariadicSignature,
        };

        using OptionalVariadicParser = Consolpp::Parsing::Parser<optionalVariadicDefinition>;
        using OptionalVariadicParameters = Consolpp::Parsing::Parameters<optionalVariadicDefinition>;
    } // namespace

    TEST(ParserTest, ParseReadsTheRequiredArgument)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"a.txt"};
        Parser parser{TokenRange{arguments}};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
        EXPECT_FALSE(parameters.getArgument<"target">().has_value());
    }

    TEST(ParserTest, ParseReadsTheOptionalArgument)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "b.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"b.txt"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getArgument<"target">(), expected);
    }

    TEST(ParserTest, ParseReadsAnEmptyArgumentAsArgument)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{""};
        Parser parser{TokenRange{arguments}};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getArgument<"source">(), "");
    }

    TEST(ParserTest, ParseReadsASingleDashAsArgument)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-"};
        Parser parser{TokenRange{arguments}};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getArgument<"source">(), "-");
    }

    TEST(ParserTest, ParseReadsEverythingAfterTheSeparatorAsArguments)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--", "--force", "-f"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"-f"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getArgument<"source">(), "--force");
        EXPECT_EQ(parameters.getArgument<"target">(), expected);
        EXPECT_FALSE(parameters.getOption<"force">().has_value());
    }

    TEST(ParserTest, ParseReadsOptionsAfterArguments)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"a.txt", "b.txt", "--force"};
        Parser parser{TokenRange{arguments}};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"force">(), std::optional<bool>{true});
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseLeavesAbsentOptionsAbsent)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"a.txt"};
        Parser parser{TokenRange{arguments}};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_FALSE(parameters.getOption<"force">().has_value());
        EXPECT_FALSE(parameters.getOption<"color">().has_value());
        EXPECT_FALSE(parameters.getOption<"output">().has_value());
        EXPECT_FALSE(parameters.getOption<"tag">().given);
    }

    TEST(ParserTest, ParseReadsALongFlag)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--force", "a.txt"};
        Parser parser{TokenRange{arguments}};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"force">(), std::optional<bool>{true});
    }

    TEST(ParserTest, ParseReadsAShortFlag)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-f", "a.txt"};
        Parser parser{TokenRange{arguments}};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"force">(), std::optional<bool>{true});
    }

    TEST(ParserTest, ParseReadsBundledShortFlags)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-fa", "a.txt"};
        Parser parser{TokenRange{arguments}};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"force">(), std::optional<bool>{true});
        EXPECT_EQ(parameters.getOption<"all">(), std::optional<bool>{true});
    }

    TEST(ParserTest, ParseReadsAGivenNegatableFlag)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--color", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<bool> expected{true};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"color">(), expected);
    }

    TEST(ParserTest, ParseReadsANegatedFlag)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--no-color", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<bool> expected{false};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"color">(), expected);
    }

    TEST(ParserTest, ParseNegatesEveryFlag)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--no-force", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<bool> expected{false};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"force">(), expected);
    }

    TEST(ParserTest, ParseReadsALongRequiredValueAfterAnEqualsSign)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--output=out.txt", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"out.txt"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"output">(), expected);
    }

    TEST(ParserTest, ParseReadsAnEmptyLongRequiredValueAfterAnEqualsSign)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--output=", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{""};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"output">(), expected);
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseReadsALongRequiredValueFromTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--output", "out.txt", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"out.txt"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"output">(), expected);
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseReadsAnEmptyNextArgumentAsRequiredValue)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--output", "", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{""};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"output">(), expected);
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseReadsAShortRequiredValueFromTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"-o", "out.txt", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"out.txt"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"output">(), expected);
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseReadsAnAttachedShortRequiredValue)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-oout.txt", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"out.txt"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"output">(), expected);
    }

    TEST(ParserTest, ParseReadsTheRestOfABundleAsValue)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-foout.txt", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"out.txt"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"force">(), std::optional<bool>{true});
        EXPECT_EQ(parameters.getOption<"output">(), expected);
    }

    TEST(ParserTest, ParseReadsTheRequiredValueOfAShortcutEndingABundleFromTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"-fo", "out.txt", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"out.txt"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"force">(), std::optional<bool>{true});
        EXPECT_EQ(parameters.getOption<"output">(), expected);
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseReadsALongOptionalValueAfterAnEqualsSign)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--tag=stable", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr OptionalValue expected{.given = true, .value = "stable"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"tag">(), expected);
    }

    TEST(ParserTest, ParseLeavesTheNextArgumentForALongOptionalValue)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--tag", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr OptionalValue expected{.given = true};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"tag">(), expected);
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseReadsAnAttachedShortOptionalValue)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-tstable", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr OptionalValue expected{.given = true, .value = "stable"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"tag">(), expected);
    }

    TEST(ParserTest, ParseLeavesTheNextArgumentForAShortOptionalValue)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-t", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr OptionalValue expected{.given = true};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"tag">(), expected);
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseThrowsForAnUnknownLongOption)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--bogus", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::UnknownOption, "bogus", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAnUnknownShortOption)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-x", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::UnknownOption, "x", 1};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAnUnknownShortOptionInsideABundle)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-fx", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::UnknownOption, "x", 1};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForANegativeNumberAsUnknownShortOption)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "-1"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::UnknownOption, "1", 1};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForANegatedOptionThatTakesAValue)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--no-output", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::UnknownOption, "no-output", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAMissingLongRequiredValueAtTheEnd)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "--output"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::MissingValue, "output", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseReadsARequiredValueStartingWithADashFromTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--output", "--force", "a.txt"};
        Parser parser{TokenRange{arguments}};
        constexpr std::optional<std::string_view> expected{"--force"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getOption<"output">(), expected);
        EXPECT_FALSE(parameters.getOption<"force">().has_value());
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseKeepsAnOptionLikeValueOfAnotherOptionOutOfARepeatableOption)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"--include=x", "--output", "--include", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const std::vector<std::string_view> expected{"x"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getOption<"include">(), expected));
        EXPECT_EQ(parameters.getOption<"output">(), std::optional<std::string_view>{"--include"});
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseThrowsForAMissingShortRequiredValue)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "-o"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::MissingValue, "o", 1};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAValueGivenToAFlag)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--force=yes", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::UnexpectedValue, "force", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAnEmptyValueGivenToAFlag)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--force=", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::UnexpectedValue, "force", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAValueGivenToANegation)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--no-color=yes", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::UnexpectedValue, "no-color", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForALongFlagGivenTwice)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--force", "a.txt", "--force"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::DuplicateOption, "force", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAFlagGivenByNameAndShortcut)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--force", "-f", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::DuplicateOption, "f", 1};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAShortFlagRepeatedInABundle)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-ff", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::DuplicateOption, "f", 1};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAFlagGivenAndNegated)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--color", "--no-color", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::DuplicateOption, "no-color", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForAFlagNegatedAndGiven)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--no-color", "--color", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::DuplicateOption, "color", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseLeavesTheCheckOfAMissingRequiredArgument)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--force"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::MissingArgument, "source", 0};
        CaughtError caught;

        const auto parameters = parser.parse();

        // Act
        try {
            parameters.requireArguments();
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseThrowsForTooManyArguments)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"a.txt", "b.txt", "c.txt"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::TooManyArguments, "c.txt", 0};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseReadsEveryValueOfARepeatableOption)
    {
        // Arrange
        constexpr std::array<const char*, 5> arguments{"--include=a", "a.txt", "-i", "b", "-ic"};
        Parser parser{TokenRange{arguments}};
        const std::vector<std::string_view> expected{"a", "b", "c"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getOption<"include">(), expected));
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseReadsNoValuesForAnAbsentRepeatableOption)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"a.txt"};
        Parser parser{TokenRange{arguments}};
        const std::vector<std::string_view> expected;

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getOption<"include">(), expected));
    }

    TEST(ParserTest, ParseReadsTheValuesOfARepeatableOptionAfterAConsumedValue)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"-o", "out.txt", "--include=a", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const std::vector<std::string_view> expected{"a"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getOption<"include">(), expected));
    }

    TEST(ParserTest, ParseReadsARepeatableOptionalValueGivenWithoutAsEmpty)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--define", "-dx", "a.txt"};
        Parser parser{TokenRange{arguments}};
        const std::vector<std::optional<std::string_view>> expected{std::nullopt, "x"};

        // Act
        const Parameters parameters = parser.parse();

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getOption<"define">(), expected));
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
    }

    TEST(ParserTest, ParseThrowsForAMissingValueOfALaterRepeatableOccurrence)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--include=a", "a.txt", "--include"};
        Parser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::MissingValue, "include", 2};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(parser.parse());
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ParseReadsEveryRemainingPositionalIntoTheVariadicArgument)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"a.txt", "b.txt", "-f", "c.txt"};
        RequiredVariadicParser parser{TokenRange{arguments}};
        const std::vector<std::string_view> expected{"b.txt", "c.txt"};

        // Act
        const RequiredVariadicParameters parameters = parser.parse();

        // Assert
        EXPECT_EQ(parameters.getArgument<"source">(), "a.txt");
        EXPECT_TRUE(std::ranges::equal(parameters.getArgument<"files">(), expected));
        EXPECT_EQ(parameters.getOption<"force">(), std::optional<bool>{true});
    }

    TEST(ParserTest, ParseLeavesOptionValuesOutOfTheVariadicArgument)
    {
        // Arrange
        constexpr std::array<const char*, 5> arguments{"a.txt", "b.txt", "-o", "out.txt", "c.txt"};
        RequiredVariadicParser parser{TokenRange{arguments}};
        const std::vector<std::string_view> expected{"b.txt", "c.txt"};
        constexpr std::optional<std::string_view> expectedOutput{"out.txt"};

        // Act
        const RequiredVariadicParameters parameters = parser.parse();

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getArgument<"files">(), expected));
        EXPECT_EQ(parameters.getOption<"output">(), expectedOutput);
    }

    TEST(ParserTest, ParseReadsOptionsAfterTheSeparatorIntoTheVariadicArgument)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"a.txt", "--", "--force", "-o"};
        RequiredVariadicParser parser{TokenRange{arguments}};
        const std::vector<std::string_view> expected{"--force", "-o"};

        // Act
        const RequiredVariadicParameters parameters = parser.parse();

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getArgument<"files">(), expected));
        EXPECT_FALSE(parameters.getOption<"force">().has_value());
    }

    TEST(ParserTest, ParseReadsNoValuesForAnOptionalVariadicArgumentNotGiven)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"a.txt"};
        OptionalVariadicParser parser{TokenRange{arguments}};
        const std::vector<std::string_view> expected;

        // Act
        const OptionalVariadicParameters parameters = parser.parse();

        // Assert
        EXPECT_TRUE(std::ranges::equal(parameters.getArgument<"files">(), expected));
    }

    TEST(ParserTest, ParseLeavesTheCheckOfARequiredVariadicArgumentWithoutValues)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "-f"};
        RequiredVariadicParser parser{TokenRange{arguments}};
        const CaughtError expected{std::in_place, ParseErrorKind::MissingArgument, "files", 0};
        CaughtError caught;

        const auto parameters = parser.parse();

        // Act
        try {
            parameters.requireArguments();
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getDashes());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }
} // namespace Consolpp::Tests::Unit::Parsing
