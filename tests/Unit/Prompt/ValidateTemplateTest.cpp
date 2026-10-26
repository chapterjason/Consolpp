#include "Consolpp/Prompt/ValidateTemplate.hpp"

#include <stdexcept>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::validateTemplate;

    TEST(ValidateTemplateTest, AcceptsAllowedPlaceholders)
    {
        // Act
        const auto validate = [] {
            validateTemplate("{label}: {message}", {"label", "message"});
        };

        // Assert
        EXPECT_NO_THROW(validate());
    }

    TEST(ValidateTemplateTest, ThrowsForAPlaceholderThatIsNotAllowed)
    {
        // Act
        const auto validate = [] {
            validateTemplate("{label}: {answer}", {"label"});
        };

        // Assert
        EXPECT_THROW(validate(), std::invalid_argument);
    }

    TEST(ValidateTemplateTest, ThrowsForAnUnclosedPlaceholder)
    {
        // Act
        const auto validate = [] {
            validateTemplate("{label", {"label"});
        };

        // Assert
        EXPECT_THROW(validate(), std::invalid_argument);
    }

    TEST(ValidateTemplateTest, ThrowsForARequiredPlaceholderTwice)
    {
        // Act
        const auto validate = [] {
            validateTemplate("{input}{input}", {"input"}, {"input"});
        };

        // Assert
        EXPECT_THROW(validate(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Prompt
