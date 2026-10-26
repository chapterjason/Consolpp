#include "Consolpp/CommandError.hpp"

#include "Consolpp/CommandErrorKind.hpp"

#include <array>
#include <exception>
#include <string_view>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    namespace
    {
        constexpr std::array<std::string_view, 3> candidates{"deploy", "deploy:all", "status"};

        bool startsWithName(const std::string_view name, const std::string_view candidate)
        {
            return candidate.starts_with(name);
        }
    } // namespace

    TEST(CommandErrorTest, DerivesFromStdException)
    {
        // Arrange
        using Base = std::exception;

        // Act
        constexpr bool derived = std::is_base_of_v<Base, CommandError>;

        // Assert
        EXPECT_TRUE(derived);
    }

    TEST(CommandErrorTest, GetKindReturnsTheKind)
    {
        // Arrange
        const CommandError error{CommandErrorKind::UnknownCommand, "deploy"};

        // Act
        const CommandErrorKind kind = error.getKind();

        // Assert
        EXPECT_EQ(kind, CommandErrorKind::UnknownCommand);
    }

    TEST(CommandErrorTest, GetSubjectReturnsTheSubject)
    {
        // Arrange
        const CommandError error{CommandErrorKind::UnknownCommand, "deploy"};

        // Act
        const std::string_view subject = error.getSubject();

        // Assert
        EXPECT_EQ(subject, "deploy");
    }

    TEST(CommandErrorTest, WhatDescribesAnUnknownCommand)
    {
        // Arrange
        const CommandError error{CommandErrorKind::UnknownCommand, "deploy"};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The command \"deploy\" does not exist");
    }

    TEST(CommandErrorTest, WhatDescribesAnUnknownNamespace)
    {
        // Arrange
        const CommandError error{CommandErrorKind::UnknownNamespace, "cache"};

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "No command is in the namespace \"cache\"");
    }

    TEST(CommandErrorTest, GetSuggestionsIsEmptyWithoutCandidates)
    {
        // Arrange
        const CommandError error{CommandErrorKind::UnknownCommand, "bogus"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view suggestion : error.createSuggestionRange()) {
            visited.push_back(suggestion);
        }

        // Assert
        EXPECT_TRUE(visited.empty());
    }

    TEST(CommandErrorTest, GetSuggestionsVisitsTheSuggestedCandidates)
    {
        // Arrange
        const CommandError error{CommandErrorKind::UnknownCommand, "dep", candidates, &startsWithName};
        const std::vector<std::string_view> expected{"deploy", "deploy:all"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view suggestion : error.createSuggestionRange()) {
            visited.push_back(suggestion);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }
} // namespace Consolpp::Tests::Unit
