#include "Consolpp/Prompt/DrawForm.hpp"

#include "../EnvironmentScope.hpp"
#include "../Stream/Channel.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/FormField.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <cstddef>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::ChoiceField;
    using Consolpp::Prompt::ChoiceFieldOptions;
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::ConfirmationFieldOptions;
    using Consolpp::Prompt::drawForm;
    using Consolpp::Prompt::FormField;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    TEST(DrawFormTest, DrawsTheActiveFieldAndTheQuestionsAfterIt)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "80"}}};
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        const std::array<FormField, 2> fields{
            FormField{TextField{TextFieldOptions{.label = "Name"}}},
            FormField{ConfirmationField{ConfirmationFieldOptions{.label = "Continue?"}}},
        };

        // Act
        const std::size_t rowsAboveCursor = drawForm(fields, 0, output, 0);

        // Assert
        EXPECT_EQ(channel.drain(), "\r\x1b[0J Name:\n > \n Continue? (yes/no): \x1b[1A\r\x1b[3C");
        EXPECT_EQ(rowsAboveCursor, 1U);
    }

    TEST(DrawFormTest, DrawsTheAnswersBeforeTheActiveField)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "80"}}};
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        TextField name{TextFieldOptions{.label = "Name"}};

        name.type(U'a');
        name.apply(Action::Submit);

        const std::array<FormField, 2> fields{
            FormField{name},
            FormField{ConfirmationField{ConfirmationFieldOptions{.label = "Continue?"}}},
        };

        // Act
        const std::size_t rowsAboveCursor = drawForm(fields, 1, output, 0);

        // Assert
        EXPECT_EQ(channel.drain(), "\r\x1b[0J Name: a\n Continue? (yes/no): \r\x1b[21C");
        EXPECT_EQ(rowsAboveCursor, 1U);
    }

    TEST(DrawFormTest, DrawsTheChoicesOfAnActiveChoiceFieldWithTheCursorAtItsQuestion)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "80"}}};
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        constexpr std::array<std::string_view, 2> choices{"apple", "banana"};
        const std::array<FormField, 2> fields{
            FormField{ChoiceField{ChoiceFieldOptions{.label = "Fruit", .choices = choices}}},
            FormField{ConfirmationField{ConfirmationFieldOptions{.label = "Continue?"}}},
        };

        // Act
        const std::size_t rowsAboveCursor = drawForm(fields, 0, output, 0);

        // Assert
        EXPECT_EQ(channel.drain(), "\r\x1b[0J Fruit:\n  > apple\n    banana\n Continue? (yes/no): \x1b[3A\r");
        EXPECT_EQ(rowsAboveCursor, 0U);
    }
} // namespace Consolpp::Tests::Integration::Prompt
