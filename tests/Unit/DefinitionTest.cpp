#include "Consolpp/Definition.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Signature.hpp"

#include <array>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{

    namespace
    {
        constexpr Consolpp::Signature signature{
            std::array{Option{.name = "force", .shortcut = 'f'}},
            std::array{Argument{.name = "source"}, Argument{.name = "target", .required = false}},
        };
    } // namespace

    TEST(DefinitionTest, DefaultsToNoAliasesUsagesHelpOrDescriptionAndVisible)
    {
        // Arrange
        constexpr std::string_view name = "deploy";

        // Act
        constexpr Consolpp::Definition command{.name = name, .signature = signature};

        // Assert
        EXPECT_EQ(command.name, "deploy");
        EXPECT_TRUE(command.description.empty());
        EXPECT_TRUE(command.aliases.empty());
        EXPECT_TRUE(command.help.empty());
        EXPECT_TRUE(command.usages.empty());
        EXPECT_FALSE(command.hidden);
    }

    TEST(DefinitionTest, DeducesItsCountsFromTheInitializer)
    {
        // Arrange
        constexpr std::array<std::string_view, 2> aliases{"d", "ship"};
        constexpr std::array<std::string_view, 1> usages{"deploy app.tar"};

        // Act
        constexpr Consolpp::Definition command{
            .name = "deploy",
            .aliases = aliases,
            .usages = usages,
            .signature = signature,
        };

        // Assert
        EXPECT_TRUE((std::is_same_v<decltype(command), const Consolpp::Definition<1, 2, 2, 1>>));
    }
} // namespace Consolpp::Tests::Unit
