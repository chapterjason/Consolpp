#include "Consolpp/Markup/Parser.hpp"

#include "Consolpp/Markup/Attribute.hpp"
#include "Consolpp/Markup/DecodeError.hpp"
#include "Consolpp/Markup/DecodeErrorKind.hpp"
#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/ParseError.hpp"
#include "Consolpp/Markup/ParseErrorKind.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Attribute;
    using Consolpp::Markup::DecodeError;
    using Consolpp::Markup::DecodeErrorKind;
    using Consolpp::Markup::Element;
    using Consolpp::Markup::ParseError;
    using Consolpp::Markup::ParseErrorKind;
    using Consolpp::Markup::Parser;

    namespace
    {
        using Segments = std::vector<std::pair<std::string, std::string>>;
        using CaughtParseError = std::optional<std::tuple<ParseErrorKind, std::string, std::size_t>>;
        using CaughtDecodeError = std::optional<std::tuple<DecodeErrorKind, std::string, std::size_t>>;

        constexpr std::string_view getName(const Element& element)
        {
            return element.name;
        }

        constexpr void ignore(const std::string_view /*text*/, const std::string_view /*element*/)
        {
        }
    } // namespace

    TEST(ParserTest, ReportsPlainTextWithTheRootContext)
    {
        // Arrange
        Parser parser{"hello"};
        const Segments expected{{"hello", "root"}};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, ReportsNothingForEmptyMarkup)
    {
        // Arrange
        Parser parser{""};
        const Segments expected{};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, ReportsTheTextAroundAnElement)
    {
        // Arrange
        Parser parser{"a<x>b</x>c"};
        const Segments expected{{"a", "root"}, {"b", "x"}, {"c", "root"}};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, AcceptsLowercaseNamesWithHyphensBetween)
    {
        // Arrange
        Parser parser{"<any-thing>t</any-thing>"};
        const Segments expected{{"t", "any-thing"}};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, UsesTheContextOfTheInnermostElement)
    {
        // Arrange
        Parser parser{"<a>1<b>2</b>3</a>"};
        const Segments expected{{"1", "a"}, {"2", "b"}, {"3", "a"}};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, KeepsEntitiesEscaped)
    {
        // Arrange
        Parser parser{"a &amp; b &#233;"};
        const Segments expected{{"a &amp; b &#233;", "root"}};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, ReportsNothingForASelfClosingElement)
    {
        // Arrange
        Parser parser{"a<x/>b"};
        const Segments expected{{"a", "root"}, {"b", "root"}};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, KeepsNewlinesInText)
    {
        // Arrange
        Parser parser{"a\n<x>b\nc</x>"};
        const Segments expected{{"a\n", "root"}, {"b\nc", "x"}};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, AllowsAGreaterThanSignInText)
    {
        // Arrange
        Parser parser{"a > b"};
        const Segments expected{{"a > b", "root"}};
        Segments segments;
        const auto visit = [&segments](const std::string_view text, const std::string_view element) {
            segments.emplace_back(text, element);
        };

        // Act
        parser.parse(std::string_view{"root"}, getName, visit);

        // Assert
        EXPECT_EQ(segments, expected);
    }

    TEST(ParserTest, PassesTheElementToTheResolver)
    {
        // Arrange
        Parser parser{"a<x/>"};
        std::vector<std::tuple<std::string, std::size_t, bool>> elements;
        const std::vector<std::tuple<std::string, std::size_t, bool>> expected{{"x", 1, true}};
        const auto resolve = [&elements](const Element& element) {
            elements.emplace_back(std::string{element.name}, element.position, element.selfClosing);

            return element.name;
        };

        // Act
        parser.parse(std::string_view{"root"}, resolve, ignore);

        // Assert
        EXPECT_EQ(elements, expected);
    }

    TEST(ParserTest, PassesTheAttributesWithTheirPositions)
    {
        // Arrange
        Parser parser{R"(<x first="1" second='two' >t</x>)"};
        std::vector<Attribute> attributes;
        const std::vector<Attribute> expected{
            {.name = "first", .namePosition = 3, .value = "1", .valuePosition = 10},
            {.name = "second", .namePosition = 13, .value = "two", .valuePosition = 21},
        };
        const auto resolve = [&attributes](const Element& element) {
            for (const Attribute& attribute : element.getAttributes()) {
                attributes.push_back(attribute);
            }

            return element.name;
        };

        // Act
        parser.parse(std::string_view{"root"}, resolve, ignore);

        // Assert
        EXPECT_EQ(attributes, expected);
    }

    TEST(ParserTest, AcceptsWhitespaceInsideTags)
    {
        // Arrange
        Parser parser{"<x\n  a = \"1\" >t</x >"};
        std::vector<std::string> values;
        const std::vector<std::string> expected{"1"};
        const auto resolve = [&values](const Element& element) {
            for (const Attribute& attribute : element.getAttributes()) {
                values.emplace_back(attribute.value);
            }

            return element.name;
        };

        // Act
        parser.parse(std::string_view{"root"}, resolve, ignore);

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(ParserTest, ThrowsForAClosingTagWithoutOpeningTag)
    {
        // Arrange
        Parser parser{"x</info>"};
        constexpr std::size_t position = 1;
        const CaughtParseError expected{std::in_place, ParseErrorKind::UnexpectedClosingTag, "info", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAMismatchedClosingTag)
    {
        // Arrange
        Parser parser{"<info>x</comment>"};
        constexpr std::size_t position = 7;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MismatchedClosingTag, "comment", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAnUnclosedElement)
    {
        // Arrange
        Parser parser{"<info>x"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::UnclosedElement, "info", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForADuplicateAttribute)
    {
        // Arrange
        Parser parser{R"(<x bold="1" bold="2">t</x>)"};
        constexpr std::size_t position = 12;
        const CaughtParseError expected{std::in_place, ParseErrorKind::DuplicateAttribute, "bold", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForALessThanSignInText)
    {
        // Arrange
        Parser parser{"a < b"};
        constexpr std::size_t position = 2;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAnUnquotedAttributeValue)
    {
        // Arrange
        Parser parser{"<x bold=true>t</x>"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<x bold=", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAttributesWithoutWhitespaceBetween)
    {
        // Arrange
        Parser parser{R"(<x a="1"b="2">t</x>)"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, R"(<x a="1")", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAnUnfinishedTag)
    {
        // Arrange
        Parser parser{"<info"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<info", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForALessThanSignInAnAttributeValue)
    {
        // Arrange
        Parser parser{R"(<x a="a<b">t</x>)"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, R"(<x a="a<b")", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAnUnterminatedAttributeValue)
    {
        // Arrange
        Parser parser{R"(<x a="1>t)"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, R"(<x a="1>t)", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAnUnknownEntityInText)
    {
        // Arrange
        Parser parser{"a &bogus; b"};
        constexpr std::size_t position = 2;
        const CaughtDecodeError expected{std::in_place, DecodeErrorKind::UnknownEntity, "&bogus;", position};
        CaughtDecodeError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAnUnterminatedEntityInAnAttributeValue)
    {
        // Arrange
        Parser parser{R"(<x a="a&b">t</x>)"};
        constexpr std::size_t position = 7;
        const CaughtDecodeError expected{std::in_place, DecodeErrorKind::UnterminatedEntity, "&", position};
        CaughtDecodeError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAnUppercaseName)
    {
        // Arrange
        Parser parser{"<Info>t</Info>"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForADigitInAName)
    {
        // Arrange
        Parser parser{"<x1>t</x1>"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<x", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForANameStartingWithAHyphen)
    {
        // Arrange
        Parser parser{"<-x>t</-x>"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<-x", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForANameEndingWithAHyphen)
    {
        // Arrange
        Parser parser{"<x->t</x->"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<x-", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForTwoHyphensInARow)
    {
        // Arrange
        Parser parser{"<a--b>t</a--b>"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<a--b", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, ThrowsForAnUppercaseAttributeName)
    {
        // Arrange
        Parser parser{R"(<x Bold="1">t</x>)"};
        constexpr std::size_t position = 0;
        const CaughtParseError expected{std::in_place, ParseErrorKind::MalformedTag, "<x ", position};
        CaughtParseError caught;

        // Act
        try {
            parser.parse(std::string_view{"root"}, getName, ignore);
        } catch (const ParseError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(ParserTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr auto countSegments = [] {
            std::size_t count = 0;
            Parser parser{"a<x>b<y>c</y></x>"};

            parser.parse(std::string_view{"root"},
                         getName,
                         [&count](const std::string_view /*text*/, const std::string_view /*element*/) {
                             ++count;
                         });

            return count;
        };

        // Act
        constexpr std::size_t count = countSegments();

        // Assert
        EXPECT_EQ(count, 3U);
    }
} // namespace Consolpp::Tests::Unit::Markup
