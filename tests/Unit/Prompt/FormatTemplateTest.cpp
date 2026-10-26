#include "Consolpp/Prompt/FormatTemplate.hpp"

#include "Consolpp/Prompt/Replacement.hpp"

#include <iterator>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::formatTemplate;
    using Consolpp::Prompt::Replacement;

    TEST(FormatTemplateTest, ReplacesEveryPlaceholderByItsValue)
    {
        // Arrange
        std::string formatted;

        // Act
        formatTemplate("a{first}b{second}c{first}",
                       {Replacement{.name = "first", .value = "1"}, Replacement{.name = "second", .value = "2"}},
                       std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "a1b2c1");
    }

    TEST(FormatTemplateTest, ThrowsForAPlaceholderWithoutReplacement)
    {
        // Arrange
        std::string formatted;

        // Act
        const auto format = [&formatted] {
            formatTemplate("a{missing}", {Replacement{.name = "first", .value = "1"}}, std::back_inserter(formatted));
        };

        // Assert
        EXPECT_THROW(format(), std::invalid_argument);
    }

    TEST(FormatTemplateTest, ThrowsForAnUnclosedPlaceholder)
    {
        // Arrange
        std::string formatted;

        // Act
        const auto format = [&formatted] {
            formatTemplate("a{first", {Replacement{.name = "first", .value = "1"}}, std::back_inserter(formatted));
        };

        // Assert
        EXPECT_THROW(format(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Prompt
