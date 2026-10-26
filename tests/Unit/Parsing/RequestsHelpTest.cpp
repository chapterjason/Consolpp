#include "Consolpp/Parsing/RequestsHelp.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <array>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::requestsHelp;
    using Consolpp::Parsing::TokenRange;

    namespace
    {
        constexpr std::array<Option, 3> options{
            {
                {.name = "force", .shortcut = 'f'},
                {.name = "output", .shortcut = 'o', .value = OptionValue::Required},
                {.name = "tag", .shortcut = 't', .value = OptionValue::Optional},
            },
        };
    } // namespace

    TEST(RequestsHelpTest, FindsTheLongOption)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "--help"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_TRUE(requested);
    }

    TEST(RequestsHelpTest, FindsTheShortcut)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-h"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_TRUE(requested);
    }

    TEST(RequestsHelpTest, FindsTheShortcutInABundle)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-fh"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_TRUE(requested);
    }

    TEST(RequestsHelpTest, IgnoresAnHInTheValueOfAShortcut)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-ohello"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsHelpTest, IgnoresAnHAfterAnOptionalValueShortcut)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-th"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsHelpTest, IgnoresTheValueOfALongOptionInTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--output", "-h"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsHelpTest, IgnoresTheValueOfAShortcutInTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-o", "--help"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsHelpTest, IgnoresEverythingAfterTheSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--", "--help"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsHelpTest, FindsNothingWithoutHelp)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-f", "a.txt"};

        // Act
        const bool requested = requestsHelp(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }
} // namespace Consolpp::Tests::Unit::Parsing
