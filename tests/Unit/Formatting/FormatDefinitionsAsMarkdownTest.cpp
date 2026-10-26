#include "Consolpp/Formatting/FormatDefinitionsAsMarkdown.hpp"

#include "Consolpp/DefinitionView.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatDefinitionsAsMarkdown;

    namespace
    {
        constexpr std::array<std::string_view, 2> aliases{"d", "ship"};
        constexpr std::array<DefinitionView, 4> definitions{
            {
                {.name = "deploy", .description = "Deploys *all*", .aliases = aliases},
                {.name = "status"},
                {.name = "cache:clear", .description = "Clears the cache"},
                {.name = "secret", .hidden = true},
            },
        };
    } // namespace

    TEST(FormatDefinitionsAsMarkdownTest, GroupsTheVisibleDefinitionsByNamespace)
    {
        // Arrange
        constexpr std::string_view namespaceName{};
        std::string formatted;

        // Act
        formatDefinitionsAsMarkdown(definitions, namespaceName, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "- `deploy` (aliases `d`, `ship`): Deploys \\*all\\*\n- `status`\n\n### cache\n\n- `cache:clear`: "
                  "Clears the cache\n");
    }

    TEST(FormatDefinitionsAsMarkdownTest, ListsOnlyTheNamespaceWithoutAHeading)
    {
        // Arrange
        constexpr std::string_view namespaceName = "cache";
        std::string formatted;

        // Act
        formatDefinitionsAsMarkdown(definitions, namespaceName, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "- `cache:clear`: Clears the cache\n");
    }

    TEST(FormatDefinitionsAsMarkdownTest, StripsTheMarkupOfTheDescriptions)
    {
        // Arrange
        constexpr std::array<DefinitionView, 2> markupDefinitions{
            {
                {.name = "empty", .description = "<info></info>"},
                {.name = "status", .description = "Shows <info>all</info> &amp; more"},
            },
        };
        constexpr std::string_view namespaceName{};
        std::string formatted;

        // Act
        formatDefinitionsAsMarkdown(markupDefinitions, namespaceName, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "- `empty`\n- `status`: Shows all & more\n");
    }
} // namespace Consolpp::Tests::Unit::Formatting
