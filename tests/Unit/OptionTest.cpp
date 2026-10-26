#include "Consolpp/Option.hpp"

#include "Consolpp/OptionValue.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    TEST(OptionTest, DefaultsToAFlagWithoutShortcut)
    {
        // Arrange
        constexpr std::string_view name = "verbose";

        // Act
        constexpr Consolpp::Option option{.name = name};

        // Assert
        EXPECT_EQ(option.name, "verbose");
        EXPECT_FALSE(option.shortcut.has_value());
        EXPECT_EQ(option.value, Consolpp::OptionValue::None);
        EXPECT_FALSE(option.repeatable);
        EXPECT_TRUE(option.description.empty());
        EXPECT_FALSE(option.defaultValue.has_value());
        EXPECT_TRUE(option.defaultValues.empty());
        EXPECT_FALSE(option.defaultEnabled.has_value());
        EXPECT_TRUE(option.suggestedValues.empty());
        EXPECT_EQ(option.completion, Consolpp::ValueCompletion::None);
    }

    TEST(OptionTest, KeepsTheDescription)
    {
        // Arrange
        constexpr std::string_view description = "Deploy without asking";

        // Act
        constexpr Consolpp::Option option{.name = "force", .description = description};

        // Assert
        EXPECT_EQ(option.description, "Deploy without asking");
    }
} // namespace Consolpp::Tests::Unit
