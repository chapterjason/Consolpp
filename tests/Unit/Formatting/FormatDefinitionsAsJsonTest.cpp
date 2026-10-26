#include "Consolpp/Formatting/FormatDefinitionsAsJson.hpp"

#include "Consolpp/DefinitionView.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatDefinitionsAsJson;

    namespace
    {
        constexpr std::array<std::string_view, 1> aliases{"d"};
        constexpr std::array<DefinitionView, 3> definitions{
            {
                {.name = "deploy", .aliases = aliases},
                {.name = "cache:clear"},
                {.name = "secret", .hidden = true},
            },
        };
    } // namespace

    TEST(FormatDefinitionsAsJsonTest, ListsEachVisibleDefinitionOnce)
    {
        // Arrange
        constexpr std::string_view namespaceName{};
        std::string encoded;

        // Act
        formatDefinitionsAsJson(definitions, namespaceName, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"namespace":null,"definitions":[{"name":"deploy","description":"","help":"","aliases":["d"],"usages":[],"options":[],"arguments":[],"hidden":false},{"name":"cache:clear","description":"","help":"","aliases":[],"usages":[],"options":[],"arguments":[],"hidden":false}],"globalOptions":[{"name":"help","shortcut":"h","value":"none","repeatable":false,"description":"Display help for the command","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"version","shortcut":null,"value":"none","repeatable":false,"description":"Display the program version","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"quiet","shortcut":"q","value":"none","repeatable":false,"description":"Only errors are displayed. All other output is suppressed","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"silent","shortcut":null,"value":"none","repeatable":false,"description":"Do not output any message","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"verbose","shortcut":"v","value":"none","repeatable":false,"description":"Increase the verbosity of messages: once for verbose output, twice for more, three times for debug","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"no-interaction","shortcut":"n","value":"none","repeatable":false,"description":"Do not ask any interactive question","defaultValue":null,"defaultValues":[],"defaultEnabled":null}]})");
    }

    TEST(FormatDefinitionsAsJsonTest, NamesTheNamespaceWhenFiltered)
    {
        // Arrange
        constexpr std::string_view namespaceName = "cache";
        std::string encoded;

        // Act
        formatDefinitionsAsJson(definitions, namespaceName, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"namespace":"cache","definitions":[{"name":"cache:clear","description":"","help":"","aliases":[],"usages":[],"options":[],"arguments":[],"hidden":false}],"globalOptions":[{"name":"help","shortcut":"h","value":"none","repeatable":false,"description":"Display help for the command","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"version","shortcut":null,"value":"none","repeatable":false,"description":"Display the program version","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"quiet","shortcut":"q","value":"none","repeatable":false,"description":"Only errors are displayed. All other output is suppressed","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"silent","shortcut":null,"value":"none","repeatable":false,"description":"Do not output any message","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"verbose","shortcut":"v","value":"none","repeatable":false,"description":"Increase the verbosity of messages: once for verbose output, twice for more, three times for debug","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"no-interaction","shortcut":"n","value":"none","repeatable":false,"description":"Do not ask any interactive question","defaultValue":null,"defaultValues":[],"defaultEnabled":null}]})");
    }
} // namespace Consolpp::Tests::Unit::Formatting
