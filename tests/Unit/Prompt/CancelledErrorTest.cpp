#include "Consolpp/Prompt/CancelledError.hpp"

#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::CancelledError;

    TEST(CancelledErrorTest, WhatNamesTheCancellation)
    {
        // Arrange
        const CancelledError error;

        // Act
        const std::string_view message = error.what();

        // Assert
        EXPECT_EQ(message, "The prompt was cancelled");
    }
} // namespace Consolpp::Tests::Unit::Prompt
