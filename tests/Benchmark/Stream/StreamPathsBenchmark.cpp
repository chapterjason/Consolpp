#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Stream/OutputStreamBuffer.hpp"

#include <array>
#include <cerrno>
#include <cstddef>
#include <format>
#include <fstream>
#include <ios>
#include <ostream>
#include <string_view>
#include <system_error>

#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <benchmark/utils.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#    include "Consolpp/Stream/InputStream.hpp"
#    include "Consolpp/Stream/SourceBuffer.hpp"

#    include <cstdio>
#    include <istream>
#    include <optional>
#    include <string>
#    include <system_error>

#    include <sys/mman.h>
#    include <sys/types.h>
#endif

namespace Consolpp::Tests::Benchmark::Stream
{

    namespace
    {
        constexpr int lineCount = 1000;

        void writeIostream(benchmark::State& state)
        {
            // Arrange
            std::ofstream output{"/dev/null"};

            output.exceptions(std::ios_base::failbit | std::ios_base::badbit);

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    output << "Deployed file " << index << " of " << lineCount << '\n';
                }

                output.flush();
            }
        }

        void writeWriterShift(benchmark::State& state)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Consolpp::Stream::OutputStream output{descriptor, Consolpp::Stream::FlushMode::Buffered};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    output << "Deployed file " << index << " of " << lineCount << '\n';
                }

                output.flush();
            }

            if (close(descriptor) != 0) {
                throw std::system_error{errno, std::generic_category()};
            }
        }

        void writeWriterFormat(benchmark::State& state)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Consolpp::Stream::OutputStream output{descriptor, Consolpp::Stream::FlushMode::Buffered};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    std::format_to(output.createIterator(), "Deployed file {} of {}\n", index, lineCount);
                }

                output.flush();
            }

            if (close(descriptor) != 0) {
                throw std::system_error{errno, std::generic_category()};
            }
        }

        void formatToArray(benchmark::State& state)
        {
            // Arrange
            std::array<char, 64> line{};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    char* end = std::format_to(line.data(), "Deployed file {} of {}\n", index, lineCount);

                    benchmark::DoNotOptimize(end);
                }
            }
        }

        void writeWriterOstream(benchmark::State& state)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Consolpp::Stream::OutputStream writer{descriptor, Consolpp::Stream::FlushMode::Buffered};
            Consolpp::Stream::OutputStreamBuffer buffer{writer};
            std::ostream output{&buffer};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    output << "Deployed file " << index << " of " << lineCount << '\n';
                }

                output.flush();
            }

            if (close(descriptor) != 0) {
                throw std::system_error{errno, std::generic_category()};
            }
        }

#ifdef __linux__
        constexpr std::size_t inputSize = 1048576;

        int createInput()
        {
            const int descriptor = memfd_create("consolpp-input", 0);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            std::string content;

            for (int index = 0; content.size() < inputSize; ++index) {
                content += std::format("line number {}\n", index);
            }

            std::string_view remaining{content};

            while (!remaining.empty()) {
                const ssize_t written = write(descriptor, remaining.data(), remaining.size());

                if (written < 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                remaining.remove_prefix(static_cast<std::size_t>(written));
            }

            return descriptor;
        }

        // Created once, so the memory counters of the read benchmarks do not include building the input.
        int getInput()
        {
            static const int descriptor = createInput();

            return descriptor;
        }

        void readLinesIostream(benchmark::State& state)
        {
            // Arrange
            const int descriptor = getInput();
            std::ifstream input{std::format("/proc/self/fd/{}", descriptor)};
            std::string line;

            if (!input) {
                throw std::system_error{errno, std::generic_category()};
            }

            input.exceptions(std::ios_base::badbit);

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                std::size_t total = 0;

                input.clear();

                if (!input.seekg(0)) {
                    throw std::system_error{errno, std::generic_category()};
                }

                while (std::getline(input, line)) {
                    total += line.size();
                }

                benchmark::DoNotOptimize(total);
            }
        }

        void readLinesTextReader(benchmark::State& state)
        {
            // Arrange
            const int descriptor = getInput();

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                std::size_t total = 0;

                if (lseek(descriptor, 0, SEEK_SET) < 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                Consolpp::Stream::InputStream input{descriptor};

                for (std::optional<std::string_view> line = input.readLine(); line.has_value();
                     line = input.readLine()) {
                    total += line->size();
                }

                benchmark::DoNotOptimize(total);
            }
        }

        void readLinesReaderBuffer(benchmark::State& state)
        {
            // Arrange
            const int descriptor = getInput();
            std::string line;

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                std::size_t total = 0;

                if (lseek(descriptor, 0, SEEK_SET) < 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                Consolpp::Stream::SourceBuffer buffer{descriptor};
                std::istream input{&buffer};

                while (std::getline(input, line)) {
                    total += line.size();
                }

                benchmark::DoNotOptimize(total);
            }
        }

        void readWordsIostream(benchmark::State& state)
        {
            // Arrange
            const int descriptor = getInput();
            std::ifstream input{std::format("/proc/self/fd/{}", descriptor)};
            std::string word;

            if (!input) {
                throw std::system_error{errno, std::generic_category()};
            }

            input.exceptions(std::ios_base::badbit);

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                std::size_t total = 0;

                input.clear();

                if (!input.seekg(0)) {
                    throw std::system_error{errno, std::generic_category()};
                }

                while (input >> word) {
                    total += word.size();
                }

                benchmark::DoNotOptimize(total);
            }
        }

        void readWordsTextReader(benchmark::State& state)
        {
            // Arrange
            const int descriptor = getInput();

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                std::size_t total = 0;

                if (lseek(descriptor, 0, SEEK_SET) < 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                Consolpp::Stream::InputStream input{descriptor};

                for (std::optional<std::string_view> word = input.readWord(); word.has_value();
                     word = input.readWord()) {
                    total += word->size();
                }

                benchmark::DoNotOptimize(total);
            }
        }
#endif
    } // namespace

    BENCHMARK(writeIostream);
    BENCHMARK(writeWriterShift);
    BENCHMARK(writeWriterFormat);
    BENCHMARK(formatToArray);
    BENCHMARK(writeWriterOstream);
#ifdef __linux__
    BENCHMARK(readLinesIostream);
    BENCHMARK(readLinesTextReader);
    BENCHMARK(readLinesReaderBuffer);
    BENCHMARK(readWordsIostream);
    BENCHMARK(readWordsTextReader);
#endif
} // namespace Consolpp::Tests::Benchmark::Stream
