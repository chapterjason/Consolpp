#include "Consolpp/Formatting/FormatDefinitionAsText.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatDefinitionAsText;

    namespace
    {
        constexpr std::array<std::string_view, 1> aliases{"d"};
        constexpr std::array<std::string_view, 1> usages{"deploy a.txt"};
        constexpr std::array<std::string_view, 2> tagDefaults{"a", "b"};
        constexpr std::array<Option, 3> options{
            {
                {.name = "force", .shortcut = 'f', .description = "Overwrite"},
                {
                    .name = "output",
                    .shortcut = 'o',
                    .value = OptionValue::Required,
                    .description = "The target\n  directory",
                    .defaultValue = "out",
                },
                {
                    .name = "tag",
                    .value = OptionValue::Required,
                    .repeatable = true,
                    .description = "Tags",
                    .defaultValues = tagDefaults,
                },
            },
        };
        constexpr std::array<Argument, 2> arguments{
            {
                {.name = "source", .description = "The source"},
                {.name = "rest", .required = false, .variadic = true, .description = "More"},
            },
        };
    } // namespace

    TEST(FormatDefinitionAsTextTest, FormatsEveryPartOfADefinition)
    {
        // Arrange
        constexpr DefinitionView deploy{
            .name = "deploy",
            .description = "Deploys the files",
            .help = "Copies every file.\nThen reports.",
            .aliases = aliases,
            .usages = usages,
            .options = options,
            .arguments = arguments,
        };
        std::string formatted;

        // Act
        formatDefinitionAsText(deploy, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "<comment>Description:</comment>\n"
                  "  Deploys the files\n"
                  "\n"
                  "<comment>Usage:</comment>\n"
                  "  deploy [options] [--] &lt;source&gt; [&lt;rest&gt;...]\n"
                  "  d\n"
                  "  deploy a.txt\n"
                  "\n"
                  "<comment>Arguments:</comment>\n"
                  "  <info>source</info>                  The source\n"
                  "  <info>rest</info>                    More\n"
                  "\n"
                  "<comment>Options:</comment>\n"
                  "  <info>-h, --help</info>              Display help for the command\n"
                  "  <info>    --version</info>           Display the program version\n"
                  "  <info>-q, --quiet</info>             Only errors are displayed. All other output is suppressed\n"
                  "  <info>    --silent</info>            Do not output any message\n"
                  "  <info>-v, --verbose</info>           Increase the verbosity of messages: once for verbose output, "
                  "twice for more, three times for debug\n"
                  "  <info>-n, --no-interaction</info>    Do not ask any interactive question\n"
                  "  <info>-f, --force|--no-force</info>  Overwrite\n"
                  "  <info>-o, --output=OUTPUT</info>     The target\n"
                  "                          directory<comment> [default: out]</comment>\n"
                  "  <info>    --tag=TAG</info>           Tags<comment> [default: a, b]</comment><comment> (multiple "
                  "values allowed)</comment>\n"
                  "\n"
                  "<comment>Help:</comment>\n"
                  "  Copies every file.\n"
                  "  Then reports.\n");
    }

    TEST(FormatDefinitionAsTextTest, LeavesOutWhatADefinitionDoesNotHave)
    {
        // Arrange
        constexpr DefinitionView status{.name = "status"};
        std::string formatted;

        // Act
        formatDefinitionAsText(status, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "<comment>Usage:</comment>\n"
                  "  status [options]\n"
                  "\n"
                  "<comment>Options:</comment>\n"
                  "  <info>-h, --help</info>            Display help for the command\n"
                  "  <info>    --version</info>         Display the program version\n"
                  "  <info>-q, --quiet</info>           Only errors are displayed. All other output is suppressed\n"
                  "  <info>    --silent</info>          Do not output any message\n"
                  "  <info>-v, --verbose</info>         Increase the verbosity of messages: once for verbose output, "
                  "twice for more, three times for debug\n"
                  "  <info>-n, --no-interaction</info>  Do not ask any interactive question\n");
    }

    TEST(FormatDefinitionAsTextTest, KeepsTheMarkupOfTheDescriptionAndHelp)
    {
        // Arrange
        constexpr DefinitionView status{
            .name = "status",
            .description = "Shows <info>all</info> &amp; more",
            .help = "Lists <comment>every\nfile</comment>",
        };
        std::string formatted;

        // Act
        formatDefinitionAsText(status, std::back_inserter(formatted));

        // Assert
        EXPECT_TRUE(formatted.starts_with("<comment>Description:</comment>\n  Shows <info>all</info> &amp; more\n"));
        EXPECT_TRUE(formatted.ends_with("\n<comment>Help:</comment>\n  Lists <comment>every\n  file</comment>\n"));
    }
} // namespace Consolpp::Tests::Unit::Formatting
