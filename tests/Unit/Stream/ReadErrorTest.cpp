#include "Consolpp/Stream/ReadError.hpp"

#include "Consolpp/Stream/ReadErrorKind.hpp"

#include <exception>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Stream
{
    using Consolpp::Stream::ReadError;
    using Consolpp::Stream::ReadErrorKind;

    TEST(ReadErrorTest, DerivesFromStdException)
    {
        // Arrange
        using Base = std::exception;

        // Act
        constexpr bool derived = std::is_base_of_v<Base, ReadError>;

        // Assert
        EXPECT_TRUE(derived);
    }

    TEST(ReadErrorTest, GetKindReturnsTheKind)
    {
        // Arrange
        const ReadError error{ReadErrorKind::InvalidValue, "abc"};

        // Act
        const ReadErrorKind kind = error.getKind();

        // Assert
        EXPECT_EQ(kind, ReadErrorKind::InvalidValue);
    }

    TEST(ReadErrorTest, GetSubjectReturnsTheSubject)
    {
        // Arrange
        const ReadError error{ReadErrorKind::InvalidValue, "abc"};

        // Act
        const std::string_view subject = error.getSubject();

        // Assert
        EXPECT_EQ(subject, "abc");
    }

    TEST(ReadErrorTest, WhatDescribesTheEndOfInput)
    {
        // Arrange
        const ReadError error{ReadErrorKind::EndOfInput, ""};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The input ended");
    }

    TEST(ReadErrorTest, WhatDescribesAnInvalidValue)
    {
        // Arrange
        const ReadError error{ReadErrorKind::InvalidValue, "abc"};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The value is invalid");
    }

    TEST(ReadErrorTest, WhatDescribesAValueOutOfRange)
    {
        // Arrange
        const ReadError error{ReadErrorKind::OutOfRange, "99999999999999999999"};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The value is out of range");
    }
} // namespace Consolpp::Tests::Unit::Stream
