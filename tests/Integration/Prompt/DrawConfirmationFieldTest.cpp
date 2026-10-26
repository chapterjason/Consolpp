#include "Consolpp/Prompt/DrawConfirmationField.hpp"

#include "../EnvironmentScope.hpp"
#include "../Stream/Channel.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <cstddef>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::ConfirmationFieldOptions;
    using Consolpp::Prompt::drawConfirmationField;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    TEST(DrawConfirmationFieldTest, DrawsTheQuestionWithTheDefaultAndTheTypedAnswer)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "80"}}};
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};

        field.apply(Action::Deny);

        // Act
        const std::size_t rowsAboveCursor = drawConfirmationField(field, output, 0);

        // Assert
        EXPECT_EQ(channel.drain(), "\r\x1b[0J Continue? (yes/no) [yes]: no");
        EXPECT_EQ(rowsAboveCursor, 0U);
    }
} // namespace Consolpp::Tests::Integration::Prompt
