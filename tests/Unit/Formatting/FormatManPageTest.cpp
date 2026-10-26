#include "Consolpp/Formatting/FormatManPage.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Program.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatManPage;

    namespace
    {
        constexpr std::array<std::string_view, 1> aliases{"d"};
        constexpr std::array<std::string_view, 1> usages{"deploy a.txt"};
        constexpr std::array<Option, 1> options{Option{.name = "force", .shortcut = 'f', .description = "Overwrite"}};
        constexpr std::array<Argument, 1> arguments{Argument{.name = "source", .description = "The source"}};
        constexpr std::array<DefinitionView, 2> definitions{
            {
                {
                    .name = "deploy",
                    .description = "Deploys the files",
                    .aliases = aliases,
                    .usages = usages,
                    .options = options,
                    .arguments = arguments,
                },
                {.name = "secret", .hidden = true},
            },
        };
    } // namespace

    TEST(FormatManPageTest, PutsEveryVisibleCommandInItsOwnSubsection)
    {
        // Arrange
        constexpr Program program{.name = "tool", .version = "1.2.3"};
        std::string page;

        // Act
        formatManPage(program, definitions, std::back_inserter(page));

        // Assert
        EXPECT_TRUE(
            page.starts_with(".TH \"TOOL\" \"1\" \"\" \"tool 1.2.3\" \"User Commands\"\n"
                             ".SH NAME\n"
                             "tool\n"
                             ".SH SYNOPSIS\n"
                             "\\fBtool\\fR command [options] [arguments]\n"
                             ".SH COMMANDS\n"
                             ".SS deploy\n"
                             "\\fBtool deploy\\fR [\\-h|\\-\\-help] [\\-f|\\-\\-force|\\-\\-no\\-force] [\\-\\-] "
                             "<source>\n"
                             ".PP\n"
                             "Deploys the files\n"
                             ".PP\n"
                             "Aliases: \\fBd\\fR\n"
                             ".TP\n"
                             "\\fIsource\\fR\n"
                             "The source\n"
                             ".TP\n"
                             "\\fB\\-f\\fR, \\fB\\-\\-force\\fR, \\fB\\-\\-no\\-force\\fR\n"
                             "Overwrite\n"
                             ".PP\n"
                             "Examples:\n"
                             ".nf\n"
                             "deploy a.txt\n"
                             ".fi\n"
                             ".SH GLOBAL OPTIONS\n"));
        EXPECT_EQ(page.find("secret"), std::string::npos);
    }
} // namespace Consolpp::Tests::Unit::Formatting
