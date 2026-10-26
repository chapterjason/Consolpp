#include "Consolpp/Prompt/FormatQuestionLine.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::formatQuestionLine;

    TEST(FormatQuestionLineTest, PutsTheLabelAndTheEncodedDefault)
    {
        // Arrange
        std::string formatted;

        // Act
        formatQuestionLine(" <a>{label}</a>{default}:", " [{value}]", "Name", "x<y", std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, " <a>Name</a> [x&lt;y]:");
    }

    TEST(FormatQuestionLineTest, LeavesOutTheDefaultPartWithoutADefault)
    {
        // Arrange
        std::string formatted;

        // Act
        formatQuestionLine(" <a>{label}</a>{default}:", " [{value}]", "Name", "", std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, " <a>Name</a>:");
    }
} // namespace Consolpp::Tests::Unit::Prompt
