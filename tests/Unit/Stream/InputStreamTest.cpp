#include "Consolpp/Stream/InputStream.hpp"

#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/Form.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <optional>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Stream
{
    using Consolpp::Stream::InputStream;
    using Consolpp::Stream::OutputStream;

    TEST(InputStreamTest, ReadsTheLinesOfTheText)
    {
        // Arrange
        InputStream input{std::string_view{"first\nsecond"}};

        // Act
        const std::optional<std::string_view> first = input.readLine();
        const std::optional<std::string_view> second = input.readLine();
        const std::optional<std::string_view> end = input.readLine();

        // Assert
        EXPECT_EQ(first, "first");
        EXPECT_EQ(second, "second");
        EXPECT_FALSE(end.has_value());
    }

    TEST(InputStreamTest, ExtractsValuesFromTheText)
    {
        // Arrange
        InputStream input{std::string_view{"42 true word"}};
        int number = 0;
        bool flag = false;
        std::string word;

        // Act
        input >> number >> flag >> word;

        // Assert
        EXPECT_EQ(number, 42);
        EXPECT_TRUE(flag);
        EXPECT_EQ(word, "word");
    }

    TEST(InputStreamTest, IsNotInteractiveForAText)
    {
        // Act
        const InputStream input{std::string_view{"answer\n"}};

        // Assert
        EXPECT_FALSE(input.isInteractive());
    }

    TEST(InputStreamTest, AsksFieldsWithTheirDefaultsForAText)
    {
        // Arrange
        std::string written;
        OutputStream output{written};
        Event::Loop loop;
        InputStream input{std::string_view{}, output, loop};
        const Prompt::Form form{
            Prompt::TextField{Prompt::TextFieldOptions{.label = "Name", .defaultValue = "app"}},
            Prompt::ConfirmationField{Prompt::ConfirmationFieldOptions{.label = "Sure?", .defaultValue = true}},
        };

        // Act
        const auto [name, confirmed] = input.ask(form);

        // Assert
        EXPECT_EQ(name, "app");
        EXPECT_TRUE(confirmed);
    }
} // namespace Consolpp::Tests::Unit::Stream
