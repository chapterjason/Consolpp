#include "Consolpp/Formatting/FormatDefinitionAsJson.hpp"

#include "Consolpp/DefinitionView.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatDefinitionAsJson;

    TEST(FormatDefinitionAsJsonTest, PutsTheGlobalOptionsInTheirOwnSection)
    {
        // Arrange
        constexpr DefinitionView definition{.name = "status"};
        std::string encoded;

        // Act
        formatDefinitionAsJson(definition, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"definition":{"name":"status","description":"","help":"","aliases":[],"usages":[],"options":[],"arguments":[],"hidden":false},"globalOptions":[{"name":"help","shortcut":"h","value":"none","repeatable":false,"description":"Display help for the command","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"version","shortcut":null,"value":"none","repeatable":false,"description":"Display the program version","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"quiet","shortcut":"q","value":"none","repeatable":false,"description":"Only errors are displayed. All other output is suppressed","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"silent","shortcut":null,"value":"none","repeatable":false,"description":"Do not output any message","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"verbose","shortcut":"v","value":"none","repeatable":false,"description":"Increase the verbosity of messages: once for verbose output, twice for more, three times for debug","defaultValue":null,"defaultValues":[],"defaultEnabled":null},{"name":"no-interaction","shortcut":"n","value":"none","repeatable":false,"description":"Do not ask any interactive question","defaultValue":null,"defaultValues":[],"defaultEnabled":null}]})");
    }
} // namespace Consolpp::Tests::Unit::Formatting
