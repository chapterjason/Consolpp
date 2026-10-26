#include "Consolpp/Formatting/CollectListEntries.hpp"

#include "Consolpp/CommandError.hpp"
#include "Consolpp/CommandErrorKind.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/ListEntry.hpp"

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::collectListEntries;
    using Consolpp::Formatting::ListEntry;

    namespace
    {
        constexpr std::array<std::string_view, 1> cacheAliases{"cc"};
        constexpr std::array<DefinitionView, 5> commands{
            {
                {.name = "cache:warmup"},
                {.name = "list"},
                {.name = "cache:clear", .aliases = cacheAliases},
                {.name = "debug:router"},
                {.name = "secret", .hidden = true},
            },
        };

        std::vector<std::string_view> getNames(const std::vector<ListEntry>& entries)
        {
            std::vector<std::string_view> names;

            names.reserve(entries.size());

            for (const ListEntry& entry : entries) {
                names.push_back(entry.name);
            }

            return names;
        }
    } // namespace

    TEST(CollectListEntriesTest, ListsNamesAndAliasesWithoutNamespaceFirst)
    {
        // Arrange
        const std::vector<std::string_view> expected{"cc", "list", "cache:clear", "cache:warmup", "debug:router"};

        // Act
        const std::vector<ListEntry> entries = collectListEntries(commands, std::string_view{});

        // Assert
        EXPECT_EQ(getNames(entries), expected);
    }

    TEST(CollectListEntriesTest, PointsAnAliasToItsCommand)
    {
        // Arrange
        constexpr std::string_view namespaceName{};

        // Act
        const std::vector<ListEntry> entries = collectListEntries(commands, namespaceName);

        // Assert
        EXPECT_EQ(entries.front().command->name, "cache:clear");
    }

    TEST(CollectListEntriesTest, KeepsOnlyTheNamesInTheNamespace)
    {
        // Arrange
        const std::vector<std::string_view> expected{"cache:clear", "cache:warmup"};

        // Act
        const std::vector<ListEntry> entries = collectListEntries(commands, "cache");

        // Assert
        EXPECT_EQ(getNames(entries), expected);
    }

    TEST(CollectListEntriesTest, ThrowsForANamespaceWithoutCommands)
    {
        // Arrange
        constexpr std::string_view namespaceName = "bogus";
        std::optional<Consolpp::CommandErrorKind> kind;

        // Act
        try {
            static_cast<void>(collectListEntries(commands, namespaceName));
        } catch (const Consolpp::CommandError& error) {
            kind = error.getKind();
        }

        // Assert
        EXPECT_EQ(kind, std::optional<Consolpp::CommandErrorKind>{Consolpp::CommandErrorKind::UnknownNamespace});
    }
} // namespace Consolpp::Tests::Unit::Formatting
