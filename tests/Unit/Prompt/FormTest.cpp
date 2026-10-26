#include "Consolpp/Prompt/Form.hpp"

#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/FormField.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"

#include <span>
#include <tuple>
#include <variant>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::ConfirmationFieldOptions;
    using Consolpp::Prompt::Form;
    using Consolpp::Prompt::FormField;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;

    TEST(FormTest, KeepsTheFieldsInTheirOrder)
    {
        // Arrange
        const TextField name{TextFieldOptions{.label = "Name"}};
        const ConfirmationField confirm{ConfirmationFieldOptions{.label = "Continue?"}};

        // Act
        const Form form{name, confirm};

        // Assert
        const std::span<const FormField> fields = form.getFormFields();
        ASSERT_EQ(fields.size(), 2U);
        EXPECT_TRUE(std::holds_alternative<TextField>(fields.front()));
        EXPECT_TRUE(std::holds_alternative<ConfirmationField>(fields.back()));
        EXPECT_EQ(std::get<0>(form.getFields()).getOptions().label, "Name");
        EXPECT_EQ(std::get<1>(form.getFields()).getOptions().label, "Continue?");
    }
} // namespace Consolpp::Tests::Unit::Prompt
