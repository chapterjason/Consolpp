#include "Consolpp/Formatting/FormatManPageHeader.hpp"

#include "Consolpp/Program.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatManPageHeader;

    TEST(FormatManPageHeaderTest, PutsTheUppercaseTitleAndTheProgramWithItsVersion)
    {
        // Arrange
        constexpr Program program{.name = "tool", .version = "2.0"};
        std::string formatted;

        // Act
        formatManPageHeader("tool-cache:clear", program, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, ".TH \"TOOL\\-CACHE:CLEAR\" \"1\" \"\" \"tool 2.0\" \"User Commands\"\n");
    }
} // namespace Consolpp::Tests::Unit::Formatting
