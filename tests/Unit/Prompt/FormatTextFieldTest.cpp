#include "Consolpp/Prompt/FormatTextField.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/FormattedTextField.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"

#include <cstddef>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::FormattedTextField;
    using Consolpp::Prompt::formatTextField;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;

    namespace
    {
        constexpr std::size_t columns = 80;
    } // namespace

    TEST(FormatTextFieldTest, FormatsTheQuestionAndTheInputWithTheCursorColumn)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .defaultValue = "app"}};

        field.type(U'a');
        field.type(U'b');
        field.apply(Action::MoveLeft);

        // Act
        const FormattedTextField formatted = formatTextField(field, columns);

        // Assert
        EXPECT_EQ(formatted.question, " <prompt-label>Name</prompt-label> [<prompt-default>app</prompt-default>]:");
        EXPECT_EQ(formatted.input, " &gt; ab");
        EXPECT_EQ(formatted.cursorColumn, 4U);
    }
} // namespace Consolpp::Tests::Unit::Prompt
