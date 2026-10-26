#include "Consolpp/Prompt/DrawTextField.hpp"

#include "../EnvironmentScope.hpp"
#include "../Stream/Channel.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <cstddef>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::drawTextField;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    TEST(DrawTextFieldTest, DrawsTheLinesAndPlacesTheCursor)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "80"}}};
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        TextField field{TextFieldOptions{.label = "Name"}};

        field.type(U'a');
        field.type(U'b');
        field.apply(Action::MoveLeft);

        // Act
        const std::size_t rowsAboveCursor = drawTextField(field, output, 0);

        // Assert
        EXPECT_EQ(channel.drain(), "\r\x1b[0J Name:\n > ab\r\x1b[4C");
        EXPECT_EQ(rowsAboveCursor, 1U);
    }

    TEST(DrawTextFieldTest, ReplacesTheLinesDrawnBefore)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "80"}}};
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        const TextField field{TextFieldOptions{.label = "Name"}};

        // Act
        static_cast<void>(drawTextField(field, output, 1));

        // Assert
        EXPECT_EQ(channel.drain(), "\r\x1b[1A\x1b[0J Name:\n > \r\x1b[3C");
    }
} // namespace Consolpp::Tests::Integration::Prompt
