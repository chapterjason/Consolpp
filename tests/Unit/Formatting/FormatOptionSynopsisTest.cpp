#include "Consolpp/Formatting/FormatOptionSynopsis.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatOptionSynopsis;

    TEST(FormatOptionSynopsisTest, WritesAFlagWithItsNegation)
    {
        // Arrange
        constexpr Option option{.name = "force", .shortcut = 'f'};
        std::string written;

        // Act
        formatOptionSynopsis(option, true, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "[-f|--force|--no-force]");
    }

    TEST(FormatOptionSynopsisTest, WritesARequiredValueInCapitals)
    {
        // Arrange
        constexpr Option option{.name = "dry-run", .value = OptionValue::Required};
        std::string written;

        // Act
        formatOptionSynopsis(option, true, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "[--dry-run DRY-RUN]");
    }

    TEST(FormatOptionSynopsisTest, WritesAnOptionalValueInBrackets)
    {
        // Arrange
        constexpr Option option{.name = "tag", .shortcut = 't', .value = OptionValue::Optional};
        std::string written;

        // Act
        formatOptionSynopsis(option, true, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "[-t|--tag [TAG]]");
    }
} // namespace Consolpp::Tests::Unit::Formatting
