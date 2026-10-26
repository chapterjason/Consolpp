#include "Consolpp/Stream/ReadDescriptor.hpp"

#include "Channel.hpp"

#include <array>
#include <cerrno>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Stream::readDescriptor;

    TEST(ReadDescriptorTest, ReturnsTheArrivedData)
    {
        // Arrange
        const Channel channel;
        constexpr std::string_view text{"abc"};
        const int descriptor = channel.getReadEnd();
        std::array<char, 65536> buffer{};

        channel.send(text);

        // Act
        const std::size_t received = readDescriptor(descriptor, buffer);

        // Assert
        EXPECT_EQ((std::string_view{buffer.data(), received}), "abc");
    }

    TEST(ReadDescriptorTest, ReturnsAtMostTheBufferSize)
    {
        // Arrange
        const Channel channel;
        const std::string text(70000, 'x');
        constexpr std::size_t expected = 65536;
        const int descriptor = channel.getReadEnd();
        std::array<char, 65536> buffer{};

        channel.send(text);

        // Act
        const std::size_t received = readDescriptor(descriptor, buffer);

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(ReadDescriptorTest, ReturnsZeroAtTheEndOfInput)
    {
        // Arrange
        const Channel channel;
        const int descriptor = channel.getReadEnd();
        std::array<char, 65536> buffer{};

        channel.endWriting();

        // Act
        const std::size_t received = readDescriptor(descriptor, buffer);

        // Assert
        EXPECT_EQ(received, 0U);
    }

    TEST(ReadDescriptorTest, ThrowsWhenTheReadFails)
    {
        // Arrange
        constexpr int descriptor = -1;
        std::array<char, 65536> buffer{};
        const std::optional<std::error_code> expected{std::error_code{EBADF, std::generic_category()}};
        std::optional<std::error_code> error;

        // Act
        try {
            static_cast<void>(readDescriptor(descriptor, buffer));
        } catch (const std::system_error& caught) {
            error = caught.code();
        }

        // Assert
        EXPECT_EQ(error, expected);
    }
} // namespace Consolpp::Tests::Integration::Stream
