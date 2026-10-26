#include "Consolpp/Formatting/FormatManPageCommand.hpp"

#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Program.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatManPageCommand;

    TEST(FormatManPageCommandTest, StartsTheSubsectionWithTheCommandAndItsSynopsis)
    {
        // Arrange
        constexpr DefinitionView command{.name = "status", .description = "Shows the status"};
        constexpr Program program{.name = "tool"};
        std::string formatted;

        // Act
        formatManPageCommand(program, command, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, ".SS status\n\\fBtool status\\fR [\\-h|\\-\\-help]\n.PP\nShows the status\n");
    }

    TEST(FormatManPageCommandTest, StripsTheMarkupOfTheHelp)
    {
        // Arrange
        constexpr DefinitionView command{
            .name = "status",
            .description = "Shows",
            .help = "Shows <info>all</info> &amp; more",
        };
        constexpr Program program{.name = "tool"};
        std::string formatted;

        // Act
        formatManPageCommand(program, command, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, ".SS status\n\\fBtool status\\fR [\\-h|\\-\\-help]\n.PP\nShows all & more\n");
    }
} // namespace Consolpp::Tests::Unit::Formatting
