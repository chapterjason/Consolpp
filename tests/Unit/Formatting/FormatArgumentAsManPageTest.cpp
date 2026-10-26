#include "Consolpp/Formatting/FormatArgumentAsManPage.hpp"

#include "Consolpp/Argument.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatArgumentAsManPage;

    TEST(FormatArgumentAsManPageTest, ShowsWhatIsSpecialAboutTheArgument)
    {
        // Arrange
        constexpr Argument argument{.name = "files", .required = false, .variadic = true, .description = "The files"};
        std::string formatted;

        // Act
        formatArgumentAsManPage(argument, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, ".TP\n\\fIfiles\\fR (optional) (multiple)\nThe files\n");
    }

    TEST(FormatArgumentAsManPageTest, StripsTheMarkupOfTheDescription)
    {
        // Arrange
        constexpr Argument argument{.name = "source", .description = "The <info>source</info>"};
        std::string formatted;

        // Act
        formatArgumentAsManPage(argument, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, ".TP\n\\fIsource\\fR\nThe source\n");
    }
} // namespace Consolpp::Tests::Unit::Formatting
