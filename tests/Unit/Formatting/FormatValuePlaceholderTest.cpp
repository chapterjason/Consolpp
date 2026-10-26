#include "Consolpp/Formatting/FormatValuePlaceholder.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatValuePlaceholder;

    TEST(FormatValuePlaceholderTest, WritesTheOptionNameInUpperCase)
    {
        // Arrange
        std::string written;

        // Act
        formatValuePlaceholder("dry-run2", std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "DRY-RUN2");
    }
} // namespace Consolpp::Tests::Unit::Formatting
