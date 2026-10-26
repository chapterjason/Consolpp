#include "Consolpp/Markup/ParseError.hpp"

#include "Consolpp/Markup/ParseErrorKind.hpp"

#include <cstddef>
#include <exception>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::ParseError;
    using Consolpp::Markup::ParseErrorKind;

    TEST(ParseErrorTest, DerivesFromStdException)
    {
        // Arrange
        using Base = std::exception;

        // Act
        constexpr bool derived = std::is_base_of_v<Base, ParseError>;

        // Assert
        EXPECT_TRUE(derived);
    }

    TEST(ParseErrorTest, KeepsKindSubjectAndPosition)
    {
        // Arrange
        const ParseError error{ParseErrorKind::MalformedTag, "x", 3};

        // Act
        const std::size_t position = error.getPosition();

        // Assert
        EXPECT_EQ(error.getKind(), ParseErrorKind::MalformedTag);
        EXPECT_EQ(error.getSubject(), "x");
        EXPECT_EQ(position, 3U);
    }

    TEST(ParseErrorTest, WhatNamesMalformedTag)
    {
        // Arrange
        const ParseError error{ParseErrorKind::MalformedTag, "<x", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The tag "<x" at position 7 is malformed)");
    }

    TEST(ParseErrorTest, WhatNamesDuplicateAttribute)
    {
        // Arrange
        const ParseError error{ParseErrorKind::DuplicateAttribute, "bold", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The attribute "bold" at position 7 is given more than once)");
    }

    TEST(ParseErrorTest, WhatNamesUnexpectedClosingTag)
    {
        // Arrange
        const ParseError error{ParseErrorKind::UnexpectedClosingTag, "info", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The closing tag "info" at position 7 has no opening tag)");
    }

    TEST(ParseErrorTest, WhatNamesMismatchedClosingTag)
    {
        // Arrange
        const ParseError error{ParseErrorKind::MismatchedClosingTag, "info", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The closing tag "info" at position 7 does not match the opening tag)");
    }

    TEST(ParseErrorTest, WhatNamesUnclosedElement)
    {
        // Arrange
        const ParseError error{ParseErrorKind::UnclosedElement, "info", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The element "info" at position 7 is not closed)");
    }
} // namespace Consolpp::Tests::Unit::Markup
