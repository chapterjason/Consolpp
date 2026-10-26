#include "Consolpp/Formatting/FormatDefinitionAsMarkdown.hpp"

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
    using Consolpp::Formatting::formatDefinitionAsMarkdown;

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
        constexpr std::array<Option, 1> markupOptions{
            {{.name = "force", .description = "<info>Overwrite</info> files"}},
        };
        constexpr std::array<Argument, 1> markupArguments{
            {{.name = "source", .description = "The <comment>source</comment>"}},
        };
    } // namespace

    TEST(FormatDefinitionAsMarkdownTest, FormatsEveryPartOfADefinition)
    {
        // Arrange
        constexpr DefinitionView deploy{
            .name = "deploy",
            .description = "Deploys the files",
            .help = "Copies every file_now.",
            .aliases = aliases,
            .usages = usages,
            .options = options,
            .arguments = arguments,
        };
        std::string formatted;

        // Act
        formatDefinitionAsMarkdown(deploy, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "## `deploy`\n"
                  "\n"
                  "Deploys the files\n"
                  "\n"
                  "### Usage\n"
                  "\n"
                  "- `deploy [options] [--] <source> [<rest>...]`\n"
                  "- `deploy a.txt`\n"
                  "\n"
                  "Aliases: `d`\n"
                  "\n"
                  "### Arguments\n"
                  "\n"
                  "- `source`: The source\n"
                  "- `rest` (optional, multiple): More\n"
                  "\n"
                  "### Options\n"
                  "\n"
                  "- `-h`, `--help`: Display help for the command\n"
                  "- `--version`: Display the program version\n"
                  "- `-q`, `--quiet`: Only errors are displayed. All other output is suppressed\n"
                  "- `--silent`: Do not output any message\n"
                  "- `-v`, `--verbose`: Increase the verbosity of messages: once for verbose output, twice for more, "
                  "three times for debug\n"
                  "- `-n`, `--no-interaction`: Do not ask any interactive question\n"
                  "- `-f`, `--force` (negatable with `--no-force`): Overwrite\n"
                  "- `-o`, `--output=OUTPUT` (default `out`): The target\n"
                  "  directory\n"
                  "- `--tag=TAG` (multiple, default `a`, `b`): Tags\n"
                  "\n"
                  "### Help\n"
                  "\n"
                  "Copies every file\\_now.\n");
    }

    TEST(FormatDefinitionAsMarkdownTest, LeavesOutWhatADefinitionDoesNotHave)
    {
        // Arrange
        constexpr DefinitionView status{.name = "status"};
        std::string formatted;

        // Act
        formatDefinitionAsMarkdown(status, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "## `status`\n"
                  "\n"
                  "### Usage\n"
                  "\n"
                  "- `status [options]`\n"
                  "\n"
                  "### Options\n"
                  "\n"
                  "- `-h`, `--help`: Display help for the command\n"
                  "- `--version`: Display the program version\n"
                  "- `-q`, `--quiet`: Only errors are displayed. All other output is suppressed\n"
                  "- `--silent`: Do not output any message\n"
                  "- `-v`, `--verbose`: Increase the verbosity of messages: once for verbose output, twice for more, "
                  "three times for debug\n"
                  "- `-n`, `--no-interaction`: Do not ask any interactive question\n");
    }

    TEST(FormatDefinitionAsMarkdownTest, StripsTheMarkupOfTheTexts)
    {
        // Arrange
        constexpr DefinitionView deploy{
            .name = "deploy",
            .description = "Deploys &lt;all&gt;",
            .help = "Copies <info>every</info> file",
            .options = markupOptions,
            .arguments = markupArguments,
        };
        std::string formatted;

        // Act
        formatDefinitionAsMarkdown(deploy, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "## `deploy`\n"
                  "\n"
                  "Deploys \\<all\\>\n"
                  "\n"
                  "### Usage\n"
                  "\n"
                  "- `deploy [options] [--] <source>`\n"
                  "\n"
                  "### Arguments\n"
                  "\n"
                  "- `source`: The source\n"
                  "\n"
                  "### Options\n"
                  "\n"
                  "- `-h`, `--help`: Display help for the command\n"
                  "- `--version`: Display the program version\n"
                  "- `-q`, `--quiet`: Only errors are displayed. All other output is suppressed\n"
                  "- `--silent`: Do not output any message\n"
                  "- `-v`, `--verbose`: Increase the verbosity of messages: once for verbose output, twice for more, "
                  "three times for debug\n"
                  "- `-n`, `--no-interaction`: Do not ask any interactive question\n"
                  "- `--force` (negatable with `--no-force`): Overwrite files\n"
                  "\n"
                  "### Help\n"
                  "\n"
                  "Copies every file\n");
    }
} // namespace Consolpp::Tests::Unit::Formatting
