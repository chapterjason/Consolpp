#include "Consolpp/Parsing/RequestsVersion.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <array>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::requestsVersion;
    using Consolpp::Parsing::TokenRange;

    namespace
    {
        constexpr std::array<Option, 2> options{
            {
                {.name = "force", .shortcut = 'f'},
                {.name = "output", .shortcut = 'o', .value = OptionValue::Required},
            },
        };
    } // namespace

    TEST(RequestsVersionTest, FindsTheLongOption)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "--version"};

        // Act
        const bool requested = requestsVersion(options, TokenRange{arguments});

        // Assert
        EXPECT_TRUE(requested);
    }

    TEST(RequestsVersionTest, HasNoShortcut)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-V"};

        // Act
        const bool requested = requestsVersion(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsVersionTest, IgnoresTheValueOfALongOptionInTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--output", "--version"};

        // Act
        const bool requested = requestsVersion(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsVersionTest, IgnoresTheValueOfAShortcutInTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-o", "--version"};

        // Act
        const bool requested = requestsVersion(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsVersionTest, IgnoresEverythingAfterTheSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--", "--version"};

        // Act
        const bool requested = requestsVersion(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }

    TEST(RequestsVersionTest, FindsNothingWithoutVersion)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-f", "a.txt"};

        // Act
        const bool requested = requestsVersion(options, TokenRange{arguments});

        // Assert
        EXPECT_FALSE(requested);
    }
} // namespace Consolpp::Tests::Unit::Parsing
