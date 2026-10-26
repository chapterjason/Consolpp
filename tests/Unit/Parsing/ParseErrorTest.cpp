#include "Consolpp/Parsing/ParseError.hpp"

#include "Consolpp/Parsing/ParseErrorKind.hpp"

#include <cstdint>
#include <exception>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::ParseError;
    using Consolpp::Parsing::ParseErrorKind;

    TEST(ParseErrorTest, DerivesFromStdException)
    {
        // Arrange
        using Base = std::exception;

        // Act
        constexpr bool derived = std::is_base_of_v<Base, ParseError>;

        // Assert
        EXPECT_TRUE(derived);
    }

    TEST(ParseErrorTest, GetKindReturnsTheKind)
    {
        // Arrange
        const ParseError error{ParseErrorKind::MissingValue, "output", 2};

        // Act
        const ParseErrorKind kind = error.getKind();

        // Assert
        EXPECT_EQ(kind, ParseErrorKind::MissingValue);
    }

    TEST(ParseErrorTest, GetSubjectReturnsTheSubject)
    {
        // Arrange
        const ParseError error{ParseErrorKind::MissingValue, "output", 2};

        // Act
        const std::string_view subject = error.getSubject();

        // Assert
        EXPECT_EQ(subject, "output");
    }

    TEST(ParseErrorTest, GetDashesReturnsTheDashes)
    {
        // Arrange
        const ParseError error{ParseErrorKind::MissingValue, "o", 1};

        // Act
        const std::uint8_t dashes = error.getDashes();

        // Assert
        EXPECT_EQ(dashes, 1);
    }

    TEST(ParseErrorTest, WhatDescribesAnUnknownOption)
    {
        // Arrange
        const ParseError error{ParseErrorKind::UnknownOption, "bogus", 2};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The option \"--bogus\" does not exist");
    }

    TEST(ParseErrorTest, WhatDescribesAMissingValue)
    {
        // Arrange
        const ParseError error{ParseErrorKind::MissingValue, "output", 2};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The option \"--output\" requires a value");
    }

    TEST(ParseErrorTest, WhatDescribesAnUnexpectedValue)
    {
        // Arrange
        const ParseError error{ParseErrorKind::UnexpectedValue, "force", 2};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The option \"--force\" does not accept a value");
    }

    TEST(ParseErrorTest, WhatDescribesADuplicateOption)
    {
        // Arrange
        const ParseError error{ParseErrorKind::DuplicateOption, "force", 2};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The option \"--force\" is given more than once");
    }

    TEST(ParseErrorTest, WhatDescribesAMissingArgument)
    {
        // Arrange
        const ParseError error{ParseErrorKind::MissingArgument, "source", 0};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The argument \"source\" is missing");
    }

    TEST(ParseErrorTest, WhatDescribesTooManyArguments)
    {
        // Arrange
        const ParseError error{ParseErrorKind::TooManyArguments, "extra", 0};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The argument \"extra\" is not expected");
    }
} // namespace Consolpp::Tests::Unit::Parsing
