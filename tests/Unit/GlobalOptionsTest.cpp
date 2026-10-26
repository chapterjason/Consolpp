#include "Consolpp/GlobalOptions.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"

#include <algorithm>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    TEST(GlobalOptionsTest, AreFlags)
    {
        // Act
        const bool flags = std::ranges::all_of(globalOptions, [](const Option& option) {
            return option.value == OptionValue::None;
        });

        // Assert
        EXPECT_TRUE(flags);
    }

    TEST(GlobalOptionsTest, StartWithHelp)
    {
        // Act
        const Option& first = globalOptions.front();

        // Assert
        EXPECT_EQ(first.name, "help");
        EXPECT_EQ(first.shortcut, 'h');
    }
} // namespace Consolpp::Tests::Unit
