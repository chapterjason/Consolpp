#include "Consolpp/Formatting/FormatOptionAsManPage.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatOptionAsManPage;

    namespace
    {
        constexpr std::array<std::string_view, 2> tags{"a", "b"};
    } // namespace

    TEST(FormatOptionAsManPageTest, ShowsTheValueAndTheDefault)
    {
        // Arrange
        constexpr Option option{
            .name = "output",
            .shortcut = 'o',
            .value = OptionValue::Required,
            .description = "The target",
            .defaultValue = "out",
        };
        std::string formatted;

        // Act
        formatOptionAsManPage(option, true, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, ".TP\n\\fB\\-o\\fR, \\fB\\-\\-output\\fR=\\fIOUTPUT\\fR\nThe target (default: out)\n");
    }

    TEST(FormatOptionAsManPageTest, ShowsSeveralDefaultsOfARepeatableOption)
    {
        // Arrange
        constexpr Option option{
            .name = "tag",
            .value = OptionValue::Optional,
            .repeatable = true,
            .description = "Tags",
            .defaultValues = tags,
        };
        std::string formatted;

        // Act
        formatOptionAsManPage(option, true, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, ".TP\n\\fB\\-\\-tag\\fR[=\\fITAG\\fR]\nTags (default: a, b) (multiple values allowed)\n");
    }

    TEST(FormatOptionAsManPageTest, StripsTheMarkupOfTheDescription)
    {
        // Arrange
        constexpr Option option{.name = "force", .description = "<info>Overwrite</info> files"};
        std::string formatted;

        // Act
        formatOptionAsManPage(option, true, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, ".TP\n\\fB\\-\\-force\\fR, \\fB\\-\\-no\\-force\\fR\nOverwrite files\n");
    }
} // namespace Consolpp::Tests::Unit::Formatting
