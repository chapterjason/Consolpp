#include "Consolpp/Stream/PlainArgument.hpp"

#include <format>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Stream
{
    using Consolpp::Stream::PlainArgument;

    TEST(PlainArgumentTest, FormatsTheValueAsEncodedMarkupText)
    {
        // Arrange
        const std::string value = "<b>&";
        const PlainArgument<std::string> argument{&value};

        // Act
        const std::string formatted = std::format("{}", argument);

        // Assert
        EXPECT_EQ(formatted, "&lt;b&gt;&amp;");
    }

    TEST(PlainArgumentTest, AppliesTheFormatSpecificationBeforeEncoding)
    {
        // Arrange
        constexpr int value = 7;
        const PlainArgument<int> argument{&value};

        // Act
        const std::string formatted = std::format("{:<3}|{:>3}", argument, argument);

        // Assert
        EXPECT_EQ(formatted, "7  |  7");
    }
} // namespace Consolpp::Tests::Unit::Stream
