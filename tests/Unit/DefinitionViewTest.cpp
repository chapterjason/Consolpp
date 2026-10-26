#include "Consolpp/DefinitionView.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Signature.hpp"

#include <array>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{

    namespace
    {
        constexpr Definition definition{
            .name = "deploy",
            .description = "Deploys the files",
            .aliases = std::array<std::string_view, 1>{"d"},
            .help = "Copies every file.",
            .usages = std::array<std::string_view, 1>{"deploy a.txt"},
            .hidden = true,
            .signature = Signature{std::array{Option{.name = "force", .shortcut = 'f'}},
                                   std::array{Argument{.name = "source"}}},
        };
    } // namespace

    TEST(DefinitionViewTest, FromDefinitionKeepsTheTexts)
    {
        // Arrange
        constexpr const auto& command = definition;

        // Act
        constexpr DefinitionView view = DefinitionView::fromDefinition(command);

        // Assert
        EXPECT_EQ(view.name, "deploy");
        EXPECT_EQ(view.description, "Deploys the files");
        EXPECT_EQ(view.help, "Copies every file.");
        EXPECT_TRUE(view.hidden);
    }

    TEST(DefinitionViewTest, FromDefinitionPointsToTheAliasesAndUsages)
    {
        // Arrange
        constexpr const auto& command = definition;

        // Act
        constexpr DefinitionView view = DefinitionView::fromDefinition(command);

        // Assert
        ASSERT_EQ(view.aliases.size(), 1U);
        EXPECT_EQ(view.aliases.front(), "d");
        ASSERT_EQ(view.usages.size(), 1U);
        EXPECT_EQ(view.usages.front(), "deploy a.txt");
    }

    TEST(DefinitionViewTest, FromDefinitionPointsToTheSignature)
    {
        // Arrange
        constexpr const auto& command = definition;

        // Act
        constexpr DefinitionView view = DefinitionView::fromDefinition(command);

        // Assert
        ASSERT_EQ(view.options.size(), 1U);
        EXPECT_EQ(view.options.front().name, "force");
        ASSERT_EQ(view.arguments.size(), 1U);
        EXPECT_EQ(view.arguments.front().name, "source");
    }
} // namespace Consolpp::Tests::Unit
