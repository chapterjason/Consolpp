#include "Consolpp/Environment/GetValue.hpp"

#include <optional>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Environment
{
    using Consolpp::Environment::getValue;

    TEST(GetValueTest, ReturnsASetVariable)
    {
        // Arrange
        constexpr const char* name = "PATH";

        // Act
        const std::optional<std::string_view> value = getValue(name);

        // Assert
        EXPECT_TRUE(value.has_value());
    }

    TEST(GetValueTest, HasNoValueForAnUnsetVariable)
    {
        // Arrange
        constexpr const char* name = "CONSOLPP_UNSET_VARIABLE_FOR_TESTS";

        // Act
        const std::optional<std::string_view> value = getValue(name);

        // Assert
        EXPECT_FALSE(value.has_value());
    }
} // namespace Consolpp::Tests::Integration::Environment
