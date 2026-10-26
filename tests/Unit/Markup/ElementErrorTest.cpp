#include "Consolpp/Markup/ElementError.hpp"

#include "Consolpp/Markup/ElementErrorKind.hpp"

#include <cstddef>
#include <exception>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::ElementError;
    using Consolpp::Markup::ElementErrorKind;

    TEST(ElementErrorTest, DerivesFromStdException)
    {
        // Arrange
        using Base = std::exception;

        // Act
        constexpr bool derived = std::is_base_of_v<Base, ElementError>;

        // Assert
        EXPECT_TRUE(derived);
    }

    TEST(ElementErrorTest, KeepsKindSubjectAndPosition)
    {
        // Arrange
        const ElementError error{ElementErrorKind::UnknownElement, "x", 3};

        // Act
        const std::size_t position = error.getPosition();

        // Assert
        EXPECT_EQ(error.getKind(), ElementErrorKind::UnknownElement);
        EXPECT_EQ(error.getSubject(), "x");
        EXPECT_EQ(position, 3U);
    }

    TEST(ElementErrorTest, WhatNamesUnknownElement)
    {
        // Arrange
        const ElementError error{ElementErrorKind::UnknownElement, "bogus", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The element "bogus" at position 7 does not exist)");
    }

    TEST(ElementErrorTest, WhatNamesUnknownAttribute)
    {
        // Arrange
        const ElementError error{ElementErrorKind::UnknownAttribute, "size", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The attribute "size" at position 7 is not allowed on its element)");
    }

    TEST(ElementErrorTest, WhatNamesMissingAttribute)
    {
        // Arrange
        const ElementError error{ElementErrorKind::MissingAttribute, "href", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The attribute "href" is missing on the element at position 7)");
    }

    TEST(ElementErrorTest, WhatNamesInvalidAttributeValue)
    {
        // Arrange
        const ElementError error{ElementErrorKind::InvalidAttributeValue, "yes", 7};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, R"(The attribute value "yes" at position 7 is invalid)");
    }
} // namespace Consolpp::Tests::Unit::Markup
