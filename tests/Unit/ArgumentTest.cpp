#include "Consolpp/Argument.hpp"

#include "Consolpp/ValueCompletion.hpp"

#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    TEST(ArgumentTest, DefaultsToRequiredAndNotVariadic)
    {
        // Arrange
        constexpr std::string_view name = "file";

        // Act
        constexpr Consolpp::Argument argument{.name = name};

        // Assert
        EXPECT_EQ(argument.name, "file");
        EXPECT_TRUE(argument.required);
        EXPECT_FALSE(argument.variadic);
        EXPECT_TRUE(argument.description.empty());
        EXPECT_FALSE(argument.defaultValue.has_value());
        EXPECT_TRUE(argument.defaultValues.empty());
        EXPECT_TRUE(argument.suggestedValues.empty());
        EXPECT_EQ(argument.completion, Consolpp::ValueCompletion::None);
    }

    TEST(ArgumentTest, KeepsTheDescription)
    {
        // Arrange
        constexpr std::string_view description = "The file to deploy";

        // Act
        constexpr Consolpp::Argument argument{.name = "file", .description = description};

        // Assert
        EXPECT_EQ(argument.description, "The file to deploy");
    }
} // namespace Consolpp::Tests::Unit
