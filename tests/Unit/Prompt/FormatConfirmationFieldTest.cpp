#include "Consolpp/Prompt/FormatConfirmationField.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"

#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::ConfirmationFieldOptions;
    using Consolpp::Prompt::formatConfirmationField;

    TEST(FormatConfirmationFieldTest, FormatsTheQuestionWithTheDefaultAndTheTypedAnswer)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};

        field.apply(Action::Deny);

        // Act
        const std::string line = formatConfirmationField(field);

        // Assert
        EXPECT_EQ(line, " <prompt-label>Continue?</prompt-label> (yes/no) [<prompt-default>yes</prompt-default>]: no");
    }

    TEST(FormatConfirmationFieldTest, LeavesTheInputEmptyWithoutATypedAnswer)
    {
        // Arrange
        const ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?"}};

        // Act
        const std::string line = formatConfirmationField(field);

        // Assert
        EXPECT_EQ(line, " <prompt-label>Continue?</prompt-label> (yes/no): ");
    }
} // namespace Consolpp::Tests::Unit::Prompt
