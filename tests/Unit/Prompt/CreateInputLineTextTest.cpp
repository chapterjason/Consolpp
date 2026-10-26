#include "Consolpp/Prompt/CreateInputLineText.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/Echo.hpp"
#include "Consolpp/Prompt/InputLineText.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::createInputLineText;
    using Consolpp::Prompt::Echo;
    using Consolpp::Prompt::InputLineText;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;

    TEST(CreateInputLineTextTest, MasksEveryGraphemeAndMovesTheCursor)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .echo = Echo::Masked}};

        field.paste("äbc");
        field.apply(Action::MoveLeft);

        // Act
        const InputLineText inputLineText = createInputLineText(field);

        // Assert
        EXPECT_EQ(inputLineText.text, "***");
        EXPECT_EQ(inputLineText.cursor, 2U);
    }

    TEST(CreateInputLineTextTest, ShowsNothingOfAHiddenText)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .echo = Echo::Hidden}};

        field.paste("abc");

        // Act
        const InputLineText inputLineText = createInputLineText(field);

        // Assert
        EXPECT_EQ(inputLineText.text, "");
        EXPECT_EQ(inputLineText.cursor, 0U);
    }
} // namespace Consolpp::Tests::Unit::Prompt
