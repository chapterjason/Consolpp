#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <cerrno>
#include <system_error>

#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <fcntl.h>
#include <unistd.h>

namespace Consolpp::Tests::Benchmark::Stream
{

    namespace
    {
        constexpr int lineCount = 1000;

        void writeLines(benchmark::State& state, const Consolpp::Stream::FlushMode mode)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Consolpp::Stream::OutputStream writer{descriptor, mode};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    writer << "Deployed file " << index << " of " << lineCount << '\n';
                }

                writer.flush();
            }

            if (close(descriptor) != 0) {
                throw std::system_error{errno, std::generic_category()};
            }
        }
    } // namespace

    BENCHMARK_CAPTURE(writeLines, buffered, Consolpp::Stream::FlushMode::Buffered);
    BENCHMARK_CAPTURE(writeLines, line, Consolpp::Stream::FlushMode::Line);
    BENCHMARK_CAPTURE(writeLines, immediate, Consolpp::Stream::FlushMode::Immediate);
} // namespace Consolpp::Tests::Benchmark::Stream
