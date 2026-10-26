#include "Consolpp/Markup/ValidateSyntax.hpp"

#include "Consolpp/Markup/DecodeError.hpp"
#include "Consolpp/Markup/ParseError.hpp"

#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::DecodeError;
    using Consolpp::Markup::ParseError;
    using Consolpp::Markup::validateSyntax;

    TEST(ValidateSyntaxTest, AcceptsTagsThatAreNotRegistered)
    {
        // Arrange
        static constexpr std::string_view markup = "<not-registered>a &amp; b</not-registered>";

        // Act
        const auto validate = [] {
            validateSyntax(markup);
        };

        // Assert
        EXPECT_NO_THROW(validate());
    }

    TEST(ValidateSyntaxTest, ThrowsForAMismatchedClosingTag)
    {
        // Arrange
        static constexpr std::string_view markup = "<info>a</comment>";

        // Act
        const auto validate = [] {
            validateSyntax(markup);
        };

        // Assert
        EXPECT_THROW(validate(), ParseError);
    }

    TEST(ValidateSyntaxTest, ThrowsForAnUnknownEntity)
    {
        // Arrange
        static constexpr std::string_view markup = "a &nbsp; b";

        // Act
        const auto validate = [] {
            validateSyntax(markup);
        };

        // Assert
        EXPECT_THROW(validate(), DecodeError);
    }
} // namespace Consolpp::Tests::Unit::Markup
