#include "Consolpp/Formatting/FormatOptionLabel.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatOptionLabel;

    TEST(FormatOptionLabelTest, WritesANegatableFlagWithShortcut)
    {
        // Arrange
        constexpr Option option{.name = "force", .shortcut = 'f'};
        constexpr bool negatable = true;
        std::string written;

        // Act
        formatOptionLabel(option, negatable, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "-f, --force|--no-force");
    }

    TEST(FormatOptionLabelTest, WritesAFlagThatCannotBeNegated)
    {
        // Arrange
        constexpr Option option{.name = "help", .shortcut = 'h'};
        constexpr bool negatable = false;
        std::string written;

        // Act
        formatOptionLabel(option, negatable, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "-h, --help");
    }

    TEST(FormatOptionLabelTest, IndentsAnOptionWithoutShortcut)
    {
        // Arrange
        constexpr Option option{.name = "output", .value = OptionValue::Required};
        constexpr bool negatable = true;
        std::string written;

        // Act
        formatOptionLabel(option, negatable, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "    --output=OUTPUT");
    }

    TEST(FormatOptionLabelTest, WritesAnOptionalValueInBrackets)
    {
        // Arrange
        constexpr Option option{.name = "dry-run", .shortcut = 'd', .value = OptionValue::Optional};
        constexpr bool negatable = true;
        std::string written;

        // Act
        formatOptionLabel(option, negatable, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "-d, --dry-run[=DRY-RUN]");
    }
} // namespace Consolpp::Tests::Unit::Formatting
