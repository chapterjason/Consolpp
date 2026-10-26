#include "Consolpp/Formatting/GetNamespace.hpp"

#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::getNamespace;

    TEST(GetNamespaceTest, ReturnsThePartBeforeTheFirstColon)
    {
        // Arrange
        constexpr std::string_view name = "cache:pool:clear";

        // Act
        const std::string_view result = getNamespace(name);

        // Assert
        EXPECT_EQ(result, "cache");
    }

    TEST(GetNamespaceTest, IsEmptyWithoutAColon)
    {
        // Arrange
        constexpr std::string_view name = "list";

        // Act
        const std::string_view result = getNamespace(name);

        // Assert
        EXPECT_TRUE(result.empty());
    }
} // namespace Consolpp::Tests::Unit::Formatting
