#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Event/Registration.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <cerrno>
#include <optional>
#include <system_error>

#include <gtest/gtest.h>
#include <unistd.h>

// POSIX SIGPIPE is not declared through <csignal>.
#include <signal.h> // IWYU pragma: keep NOLINT(modernize-deprecated-headers)

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Event::Loop;
    using Consolpp::Event::Registration;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;

    namespace
    {
        class BrokenPipe
        {
            private:
                std::array<int, 2> descriptors{-1, -1};

            public:
                BrokenPipe()
                {
                    if (pipe(this->descriptors.data()) != 0) {
                        throw std::system_error{errno, std::generic_category()};
                    }

                    if (close(std::get<0>(this->descriptors)) != 0) {
                        const int error = errno;

                        close(std::get<1>(this->descriptors));

                        throw std::system_error{error, std::generic_category()};
                    }
                }

                BrokenPipe(const BrokenPipe&) = delete;
                BrokenPipe(BrokenPipe&&) = delete;
                BrokenPipe& operator=(const BrokenPipe&) = delete;
                BrokenPipe& operator=(BrokenPipe&&) = delete;

                ~BrokenPipe()
                {
                    close(std::get<1>(this->descriptors));
                }

                [[nodiscard]]
                int getWriteEnd() const
                {
                    return std::get<1>(this->descriptors);
                }
        };
    } // namespace

    TEST(BrokenPipeTest, WritingThrowsEpipeWhenSigpipeIsRegistered)
    {
        // Arrange
        const BrokenPipe broken;
        Loop loop;
        const Registration registration = loop.registerSignal(SIGPIPE, [] {
        });
        OutputStream writer{broken.getWriteEnd(), FlushMode::Immediate};
        const std::optional<std::error_code> expected{std::error_code{EPIPE, std::generic_category()}};
        std::optional<std::error_code> error;

        // Act
        try {
            writer << "abc";
        } catch (const std::system_error& caught) {
            error = caught.code();
        }

        // Assert
        EXPECT_EQ(error, expected);
    }
} // namespace Consolpp::Tests::Integration::Stream
