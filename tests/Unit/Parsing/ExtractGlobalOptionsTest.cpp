#include "Consolpp/Parsing/ExtractGlobalOptions.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/GlobalOptionValues.hpp"
#include "Consolpp/Stream/Verbosity.hpp"

#include <array>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::extractGlobalOptions;
    using Consolpp::Parsing::GlobalOptionValues;
    using Consolpp::Stream::Verbosity;

    namespace
    {
        constexpr std::array<Option, 1> options{
            Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
        };
    } // namespace

    TEST(ExtractGlobalOptionsTest, TakesTheGlobalOptionsOut)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"a.txt", "-vv", "-n", "b.txt"};
        std::vector<const char*> remaining;

        // Act
        const GlobalOptionValues values = extractGlobalOptions(options, arguments, remaining);

        // Assert
        EXPECT_EQ(values.verbosity, std::optional<Verbosity>{Verbosity::VeryVerbose});
        EXPECT_TRUE(values.interactionDisabled);
        EXPECT_EQ(remaining, (std::vector<const char*>{"a.txt", "b.txt"}));
    }

    TEST(ExtractGlobalOptionsTest, QuietAlsoDisablesInteraction)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--quiet"};
        std::vector<const char*> remaining;

        // Act
        const GlobalOptionValues values = extractGlobalOptions(options, arguments, remaining);

        // Assert
        EXPECT_EQ(values.verbosity, std::optional<Verbosity>{Verbosity::Quiet});
        EXPECT_TRUE(values.interactionDisabled);
    }

    TEST(ExtractGlobalOptionsTest, LeavesTheValueOfAnOption)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-o", "-v"};
        std::vector<const char*> remaining;

        // Act
        const GlobalOptionValues values = extractGlobalOptions(options, arguments, remaining);

        // Assert
        EXPECT_FALSE(values.verbosity.has_value());
        EXPECT_EQ(remaining.size(), 2U);
    }

    TEST(ExtractGlobalOptionsTest, LeavesEverythingAfterTheSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--", "-q"};
        std::vector<const char*> remaining;

        // Act
        const GlobalOptionValues values = extractGlobalOptions(options, arguments, remaining);

        // Assert
        EXPECT_FALSE(values.verbosity.has_value());
        EXPECT_EQ(remaining.size(), 2U);
    }
} // namespace Consolpp::Tests::Unit::Parsing
