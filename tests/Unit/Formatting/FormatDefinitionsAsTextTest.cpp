#include "Consolpp/Formatting/FormatDefinitionsAsText.hpp"

#include "Consolpp/DefinitionView.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatDefinitionsAsText;

    namespace
    {
        constexpr std::array<std::string_view, 1> aliases{"d"};
        constexpr std::array<DefinitionView, 4> definitions{
            {
                {.name = "deploy", .description = "Deploys the files", .aliases = aliases},
                {.name = "status", .description = "Shows status"},
                {.name = "cache:clear", .description = "Clears the cache"},
                {.name = "secret", .description = "Hidden", .hidden = true},
            },
        };
    } // namespace

    TEST(FormatDefinitionsAsTextTest, GroupsTheVisibleDefinitionsByNamespace)
    {
        // Arrange
        constexpr std::string_view namespaceName{};
        std::string formatted;

        // Act
        formatDefinitionsAsText(definitions, namespaceName, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "<comment>Usage:</comment>\n"
                  "  command [options] [arguments]\n"
                  "\n"
                  "<comment>Options:</comment>\n"
                  "  <info>-h, --help</info>            Display help for the command\n"
                  "  <info>    --version</info>         Display the program version\n"
                  "  <info>-q, --quiet</info>           Only errors are displayed. All other output is suppressed\n"
                  "  <info>    --silent</info>          Do not output any message\n"
                  "  <info>-v, --verbose</info>         Increase the verbosity of messages: once for verbose output, "
                  "twice for more, three times for debug\n"
                  "  <info>-n, --no-interaction</info>  Do not ask any interactive question\n"
                  "\n"
                  "<comment>Available commands:</comment>\n"
                  "  <info>d</info>            Deploys the files\n"
                  "  <info>deploy</info>       [d] Deploys the files\n"
                  "  <info>status</info>       Shows status\n"
                  " <comment>cache</comment>\n"
                  "  <info>cache:clear</info>  Clears the cache\n");
    }

    TEST(FormatDefinitionsAsTextTest, NamesTheNamespaceWhenFiltered)
    {
        // Arrange
        constexpr std::string_view namespaceName = "cache";
        std::string formatted;

        // Act
        formatDefinitionsAsText(definitions, namespaceName, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "<comment>Usage:</comment>\n"
                  "  command [options] [arguments]\n"
                  "\n"
                  "<comment>Options:</comment>\n"
                  "  <info>-h, --help</info>            Display help for the command\n"
                  "  <info>    --version</info>         Display the program version\n"
                  "  <info>-q, --quiet</info>           Only errors are displayed. All other output is suppressed\n"
                  "  <info>    --silent</info>          Do not output any message\n"
                  "  <info>-v, --verbose</info>         Increase the verbosity of messages: once for verbose output, "
                  "twice for more, three times for debug\n"
                  "  <info>-n, --no-interaction</info>  Do not ask any interactive question\n"
                  "\n"
                  "<comment>Available commands for the \"cache\" namespace:</comment>\n"
                  "  <info>cache:clear</info>  Clears the cache\n");
    }

    TEST(FormatDefinitionsAsTextTest, KeepsTheMarkupOfTheDescriptions)
    {
        // Arrange
        constexpr std::array<DefinitionView, 1> markupDefinitions{
            {{.name = "status", .description = "Shows <info>all</info> &amp; more"}},
        };
        constexpr std::string_view namespaceName{};
        std::string formatted;

        // Act
        formatDefinitionsAsText(markupDefinitions, namespaceName, std::back_inserter(formatted));

        // Assert
        EXPECT_TRUE(formatted.ends_with("<comment>Available commands:</comment>\n"
                                        "  <info>status</info>  Shows <info>all</info> &amp; more\n"));
    }
} // namespace Consolpp::Tests::Unit::Formatting
