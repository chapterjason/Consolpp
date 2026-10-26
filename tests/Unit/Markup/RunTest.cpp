#include "Consolpp/Markup/Run.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    TEST(RunTest, StartsEmptyAtTheBeginning)
    {
        // Act
        // Unqualified, Run would be GoogleTest's Test::Run.
        constexpr Consolpp::Markup::Run<int> run{.context = 7};

        // Assert
        EXPECT_EQ(run.start, 0U);
        EXPECT_EQ(run.length, 0U);
        EXPECT_EQ(run.context, 7);
    }
} // namespace Consolpp::Tests::Unit::Markup
