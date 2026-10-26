#include "Consolpp/GetVersion.hpp"

#include "Consolpp/Version.hpp"

#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    TEST(GetVersionTest, ReturnsTheProjectVersion)
    {
        // Arrange
        constexpr std::string_view expected = version;

        // Act
        const std::string_view version = getVersion();

        // Assert
        EXPECT_EQ(version, expected);
    }
} // namespace Consolpp::Tests::Unit
