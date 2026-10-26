#include "Consolpp/Prompt/ConfirmationField.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/State.hpp"

#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::ConfirmationFieldOptions;
    using Consolpp::Prompt::State;

    TEST(ConfirmationFieldTest, SubmitsTheTypedYes)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = false}};

        field.apply(Action::Confirm);

        // Act
        field.apply(Action::Submit);

        // Assert
        EXPECT_EQ(field.getState(), State::Submitted);
        EXPECT_TRUE(field.getValue());
    }

    TEST(ConfirmationFieldTest, SubmitsTheTypedNo)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};

        field.apply(Action::Deny);

        // Act
        field.apply(Action::Submit);

        // Assert
        EXPECT_FALSE(field.getValue());
    }

    TEST(ConfirmationFieldTest, TakesTheDefaultWithoutATypedAnswer)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};

        // Act
        field.apply(Action::Submit);

        // Assert
        EXPECT_EQ(field.getState(), State::Submitted);
        EXPECT_TRUE(field.getValue());
    }

    TEST(ConfirmationFieldTest, NeedsAnAnswerWithoutADefault)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?"}};

        // Act
        const bool taken = field.apply(Action::Submit);

        // Assert
        EXPECT_FALSE(taken);
        EXPECT_EQ(field.getState(), State::Active);
    }

    TEST(ConfirmationFieldTest, CancelEndsTheField)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?"}};

        // Act
        field.apply(Action::Cancel);

        // Assert
        EXPECT_EQ(field.getState(), State::Cancelled);
    }

    TEST(ConfirmationFieldTest, ThrowsForAPlaceholderTheTemplateDoesNotAllow)
    {
        // Arrange
        const std::string_view markupTemplate{"{choice}?"};

        // Act
        const auto set = [markupTemplate] {
            ConfirmationField::setQuestionTemplate(markupTemplate);
        };

        // Assert
        EXPECT_THROW(set(), std::invalid_argument);
    }

    TEST(ConfirmationFieldTest, ReopensASubmittedField)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};

        field.apply(Action::Submit);

        // Act
        field.reopen();

        // Assert
        EXPECT_EQ(field.getState(), State::Active);
    }

    TEST(ConfirmationFieldTest, KeepsACancelledFieldCancelledWhenReopened)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};

        field.apply(Action::Cancel);

        // Act
        field.reopen();

        // Assert
        EXPECT_EQ(field.getState(), State::Cancelled);
    }

    TEST(ConfirmationFieldTest, LeavesThePreviousFieldActionToTheForm)
    {
        // Arrange
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};

        // Act
        const bool taken = field.apply(Action::PreviousField);

        // Assert
        EXPECT_FALSE(taken);
        EXPECT_EQ(field.getState(), State::Active);
    }
} // namespace Consolpp::Tests::Unit::Prompt
