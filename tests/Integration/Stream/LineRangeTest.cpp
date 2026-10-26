#include "Consolpp/Stream/LineRange.hpp"

#include "Channel.hpp"
#include "Consolpp/Stream/InputStream.hpp"

#include <cstddef>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Stream::InputStream;
    using Consolpp::Stream::LineRange;

    TEST(LineRangeTest, SatisfiesInputRange)
    {
        // Arrange
        using Range = LineRange;

        // Act
        constexpr bool satisfied = std::ranges::input_range<Range>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(LineRangeTest, RangeForVisitsEveryLine)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::vector<std::string> expected{"first", "second"};
        std::vector<std::string> visited;

        channel.send("first\nsecond\n");
        channel.endWriting();

        // Act
        for (const std::string_view line : text.createLineRange()) {
            visited.emplace_back(line);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(LineRangeTest, WorksWithViews)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        constexpr std::ptrdiff_t expected = 2;

        channel.send("first\n\nthird\n");
        channel.endWriting();

        // Act
        const std::ptrdiff_t count = std::ranges::distance(text.createLineRange()
                                                           | std::views::filter([](const std::string_view line) {
                                                                 return !line.empty();
                                                             }));

        // Assert
        EXPECT_EQ(count, expected);
    }
} // namespace Consolpp::Tests::Integration::Stream
