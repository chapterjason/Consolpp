#include "Consolpp/Markup/DecodeEntity.hpp"

#include "Consolpp/Markup/DecodeError.hpp"
#include "Consolpp/Markup/DecodeErrorKind.hpp"
#include "Consolpp/Markup/Entity.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::decodeEntity;
    using Consolpp::Markup::DecodeError;
    using Consolpp::Markup::DecodeErrorKind;
    using Consolpp::Markup::Entity;

    namespace
    {
        using CaughtError = std::optional<std::tuple<DecodeErrorKind, std::string, std::size_t>>;
    } // namespace

    TEST(DecodeEntityTest, DecodesLt)
    {
        // Arrange
        constexpr std::string_view text = "&lt;";
        constexpr Entity expected{.codePoint = '<', .length = 4};

        // Act
        const Entity decoded = decodeEntity(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeEntityTest, DecodesGt)
    {
        // Arrange
        constexpr std::string_view text = "&gt;";
        constexpr Entity expected{.codePoint = '>', .length = 4};

        // Act
        const Entity decoded = decodeEntity(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeEntityTest, DecodesAmp)
    {
        // Arrange
        constexpr std::string_view text = "&amp;";
        constexpr Entity expected{.codePoint = '&', .length = 5};

        // Act
        const Entity decoded = decodeEntity(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeEntityTest, DecodesQuot)
    {
        // Arrange
        constexpr std::string_view text = "&quot;";
        constexpr Entity expected{.codePoint = '"', .length = 6};

        // Act
        const Entity decoded = decodeEntity(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeEntityTest, DecodesApos)
    {
        // Arrange
        constexpr std::string_view text = "&apos;";
        constexpr Entity expected{.codePoint = '\'', .length = 6};

        // Act
        const Entity decoded = decodeEntity(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeEntityTest, DecodesACharacterReference)
    {
        // Arrange
        constexpr std::string_view text = "&#xE9;";
        constexpr Entity expected{.codePoint = 0xE9, .length = 6};

        // Act
        const Entity decoded = decodeEntity(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeEntityTest, StopsAtTheFirstSemicolon)
    {
        // Arrange
        constexpr std::string_view text = "&amp;rest;";
        constexpr Entity expected{.codePoint = '&', .length = 5};

        // Act
        const Entity decoded = decodeEntity(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }

    TEST(DecodeEntityTest, ThrowsForAnUnknownName)
    {
        // Arrange
        constexpr std::string_view text = "&nbsp;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::UnknownEntity, "&nbsp;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeEntity(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeEntityTest, ThrowsForAnUnterminatedEntity)
    {
        // Arrange
        constexpr std::string_view text = "&amp";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::UnterminatedEntity, "&", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeEntity(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeEntityTest, ThrowsWithoutAmpersand)
    {
        // Arrange
        constexpr std::string_view text = "amp;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::UnknownEntity, "amp;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeEntity(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeEntityTest, ThrowsForAnEmptyName)
    {
        // Arrange
        constexpr std::string_view text = "&;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::UnknownEntity, "&;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeEntity(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeEntityTest, PointsAnInvalidCharacterReferenceToTheEntity)
    {
        // Arrange
        constexpr std::string_view text = "&#0;";
        constexpr std::size_t position = 7;
        const CaughtError expected{std::in_place, DecodeErrorKind::InvalidCharacterReference, "&#0;", position};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(decodeEntity(text, position));
        } catch (const DecodeError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(DecodeEntityTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr std::string_view text = "&#xE9;";
        constexpr Entity expected{.codePoint = 0xE9, .length = 6};

        // Act
        constexpr Entity decoded = decodeEntity(text, 0);

        // Assert
        EXPECT_EQ(decoded, expected);
    }
} // namespace Consolpp::Tests::Unit::Markup
