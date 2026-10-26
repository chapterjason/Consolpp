#include "Consolpp/Markup/DecodeCharacterReference.hpp"

#include "Consolpp/Markup/DecodeError.hpp"
#include "Consolpp/Markup/DecodeErrorKind.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::decodeCharacterReference;
    using Consolpp::Markup::DecodeError;
    using Consolpp::Markup::DecodeErrorKind;

    namespace
    {
        using CaughtError = std::optional<std::tuple<DecodeErrorKind, std::string, std::size_t>>;
    } // namespace

    TEST(DecodeCharacterReferenceTest, DecodesADecimalReference)
    {
        // Arrange
        constexpr std::string_view text = "&#233;";
        constexpr std::uint32_t expected{233};

        // Act
        const std::uint32_t decoded = decodeCharacterReference(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeCharacterReferenceTest, DecodesAHexReference)
    {
        // Arrange
        constexpr std::string_view text = "&#xE9;";
        constexpr std::uint32_t expected{0xE9};

        // Act
        const std::uint32_t decoded = decodeCharacterReference(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeCharacterReferenceTest, DecodesALowercaseHexReference)
    {
        // Arrange
        constexpr std::string_view text = "&#x1f600;";
        constexpr std::uint32_t expected{0x1F600};

        // Act
        const std::uint32_t decoded = decodeCharacterReference(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeCharacterReferenceTest, DecodesTheLastCodePoint)
    {
        // Arrange
        constexpr std::string_view text = "&#x10FFFF;";
        constexpr std::uint32_t expected{0x10FFFF};

        // Act
        const std::uint32_t decoded = decodeCharacterReference(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeCharacterReferenceTest, DecodesATab)
    {
        // Arrange
        constexpr std::string_view text = "&#9;";
        constexpr std::uint32_t expected{'\t'};

        // Act
        const std::uint32_t decoded = decodeCharacterReference(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsForAnEmptyReference)
    {
        // Arrange
        constexpr std::string_view text = "&#;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsForAnEmptyHexReference)
    {
        // Arrange
        constexpr std::string_view text = "&#x;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#x;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsWithoutNumberSign)
    {
        // Arrange
        constexpr std::string_view text = "&65;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&65;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsForANonDigit)
    {
        // Arrange
        constexpr std::string_view text = "&#12a;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#12a;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsForAnUppercaseX)
    {
        // Arrange
        constexpr std::string_view text = "&#X41;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#X41;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsForNul)
    {
        // Arrange
        constexpr std::string_view text = "&#0;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#0;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsForASurrogate)
    {
        // Arrange
        constexpr std::string_view text = "&#xD800;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#xD800;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsForANonCharacter)
    {
        // Arrange
        constexpr std::string_view text = "&#xFFFE;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#xFFFE;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsBeyondTheLastCodePoint)
    {
        // Arrange
        constexpr std::string_view text = "&#x110000;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#x110000;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsWithoutSemicolon)
    {
        // Arrange
        constexpr std::string_view text = "&#65";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#65", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, ThrowsForAHugeNumber)
    {
        // Arrange
        constexpr std::string_view text = "&#99999999999999999999;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place,
                                   DecodeErrorKind::InvalidCharacterReference,
                                   "&#99999999999999999999;",
                                   position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeCharacterReference(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeCharacterReferenceTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr std::string_view text = "&#xE9;";
        constexpr std::uint32_t expected{0xE9};

        // Act
        constexpr std::uint32_t decoded = decodeCharacterReference(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }
} // namespace Consolpp::Tests::Unit::Markup
