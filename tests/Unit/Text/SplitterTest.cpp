#include "Consolpp/Text/Splitter.hpp"

#include <stdexcept>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::Splitter;

    TEST(SplitterTest, TakesThePartsBetweenSeparators)
    {
        // Arrange
        Splitter parts{"cache:pool:clear", ':'};
        const std::vector<std::string_view> expected{"cache", "pool", "clear"};
        std::vector<std::string_view> taken;

        // Act
        while (parts.hasNext()) {
            taken.push_back(parts.takeNext());
        }

        // Assert
        EXPECT_EQ(taken, expected);
    }

    TEST(SplitterTest, TakesTheWholeTextWithoutSeparator)
    {
        // Arrange
        Splitter parts{"list", ':'};
        const std::vector<std::string_view> expected{"list"};
        std::vector<std::string_view> taken;

        // Act
        while (parts.hasNext()) {
            taken.push_back(parts.takeNext());
        }

        // Assert
        EXPECT_EQ(taken, expected);
    }

    TEST(SplitterTest, TakesOneEmptyPartFromAnEmptyText)
    {
        // Arrange
        Splitter parts{"", ':'};
        const std::vector<std::string_view> expected{""};
        std::vector<std::string_view> taken;

        // Act
        while (parts.hasNext()) {
            taken.push_back(parts.takeNext());
        }

        // Assert
        EXPECT_EQ(taken, expected);
    }

    TEST(SplitterTest, TakesEmptyPartsAroundSeparators)
    {
        // Arrange
        Splitter parts{":a::b:", ':'};
        const std::vector<std::string_view> expected{"", "a", "", "b", ""};
        std::vector<std::string_view> taken;

        // Act
        while (parts.hasNext()) {
            taken.push_back(parts.takeNext());
        }

        // Assert
        EXPECT_EQ(taken, expected);
    }

    TEST(SplitterTest, ThrowsWhenTakingAfterTheLastPart)
    {
        // Arrange
        Splitter parts{"list", ':'};
        bool thrown = false;

        static_cast<void>(parts.takeNext());

        // Act
        try {
            static_cast<void>(parts.takeNext());
        } catch (const std::out_of_range&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
    }

    TEST(SplitterTest, IsUsableAtCompileTime)
    {
        // Arrange
        constexpr auto takeSecond = [] {
            Splitter parts{"cache:clear", ':'};

            static_cast<void>(parts.takeNext());

            return parts.takeNext();
        };

        // Act
        constexpr std::string_view second = takeSecond();

        // Assert
        EXPECT_EQ(second, "clear");
    }
} // namespace Consolpp::Tests::Unit::Text
