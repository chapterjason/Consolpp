#include "Consolpp/Markup/DecodeError.hpp"

#include "Consolpp/Markup/DecodeErrorKind.hpp"

#include <cstddef>
#include <exception>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::DecodeError;
    using Consolpp::Markup::DecodeErrorKind;

    TEST(DecodeErrorTest, DerivesFromStdException)
    {
        // Arrange
        using Base = std::exception;

        // Act
        constexpr bool derived = std::is_base_of_v<Base, DecodeError>;

        // Assert
        EXPECT_TRUE(derived);
    }

    TEST(DecodeErrorTest, GetKindReturnsTheKind)
    {
        // Arrange
        const DecodeError error{DecodeErrorKind::UnknownEntity, "nbsp", 3};

        // Act
        const DecodeErrorKind kind = error.getKind();

        // Assert
        EXPECT_EQ(kind, DecodeErrorKind::UnknownEntity);
    }

    TEST(DecodeErrorTest, GetSubjectReturnsTheSubject)
    {
        // Arrange
        const DecodeError error{DecodeErrorKind::UnknownEntity, "nbsp", 3};

        // Act
        const std::string_view subject = error.getSubject();

        // Assert
        EXPECT_EQ(subject, "nbsp");
    }

    TEST(DecodeErrorTest, GetPositionReturnsThePosition)
    {
        // Arrange
        const DecodeError error{DecodeErrorKind::UnknownEntity, "nbsp", 3};

        // Act
        const std::size_t position = error.getPosition();

        // Assert
        EXPECT_EQ(position, 3U);
    }

    TEST(DecodeErrorTest, WhatNamesTheUnknownEntityAndItsPosition)
    {
        // Arrange
        const DecodeError error{DecodeErrorKind::UnknownEntity, "&nbsp;", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The entity \"&nbsp;\" at position 7 does not exist");
    }

    TEST(DecodeErrorTest, WhatNamesTheUnterminatedEntityAndItsPosition)
    {
        // Arrange
        const DecodeError error{DecodeErrorKind::UnterminatedEntity, "&", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The entity at position 7 has no closing \";\"");
    }

    TEST(DecodeErrorTest, WhatNamesTheInvalidCharacterReferenceAndItsPosition)
    {
        // Arrange
        const DecodeError error{DecodeErrorKind::InvalidCharacterReference, "&#0;", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The character reference \"&#0;\" at position 7 is invalid");
    }
} // namespace Consolpp::Tests::Unit::Markup
