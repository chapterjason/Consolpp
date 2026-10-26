#include "Consolpp/BuiltIn/ParseFormat.hpp"

#include "Consolpp/BuiltIn/Format.hpp"

#include <array>
#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::BuiltIn
{
    using Consolpp::BuiltIn::Format;
    using Consolpp::BuiltIn::parseFormat;

    TEST(ParseFormatTest, ParsesTxtJsonAndMd)
    {
        // Arrange
        constexpr std::array<Format, 3> expected{Format::Text, Format::Json, Format::Markdown};

        // Act
        constexpr std::array<Format, 3> formats{parseFormat("txt"), parseFormat("json"), parseFormat("md")};

        // Assert
        EXPECT_EQ(formats, expected);
    }

    TEST(ParseFormatTest, ThrowsForAnUnknownName)
    {
        // Act
        const auto parse = [] {
            static_cast<void>(parseFormat("xml"));
        };

        // Assert
        EXPECT_THROW(parse(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::BuiltIn
