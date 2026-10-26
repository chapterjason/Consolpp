#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Styling/StyledSpan.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <cerrno>
#include <format>
#include <string_view>
#include <system_error>

#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <fcntl.h>
#include <unistd.h>

namespace Consolpp::Tests::Benchmark::Styling
{

    namespace
    {
        constexpr int lineCount = 1000;

        void writePlain(benchmark::State& state)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Consolpp::Stream::OutputStream writer{descriptor, Consolpp::Stream::FlushMode::Buffered};

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

        void writeFormattedMarkup(benchmark::State& state)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Consolpp::Stream::OutputStream output{descriptor,
                                                  Consolpp::Stream::FlushMode::Buffered,
                                                  true,
                                                  Consolpp::Terminal::Capabilities{}};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    output.write("<info>Deployed</info> file {} of {}\n", index, lineCount);
                }

                output.flush();
            }

            if (close(descriptor) != 0) {
                throw std::system_error{errno, std::generic_category()};
            }
        }

        void writeRuntimeMarkup(benchmark::State& state)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Consolpp::Stream::OutputStream output{descriptor,
                                                  Consolpp::Stream::FlushMode::Buffered,
                                                  true,
                                                  Consolpp::Terminal::Capabilities{}};
            std::array<char, 64> line{};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    auto* const end = std::format_to(line.begin(),
                                                     "<info>Deployed</info> file {} of {}\n",
                                                     index,
                                                     lineCount);

                    output << std::string_view{line.begin(), end};
                }

                output.flush();
            }

            if (close(descriptor) != 0) {
                throw std::system_error{errno, std::generic_category()};
            }
        }

        void writeTyped(benchmark::State& state)
        {
            // Arrange
            const int descriptor = open("/dev/null", O_WRONLY | O_CLOEXEC);

            if (descriptor < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            Consolpp::Stream::OutputStream output{descriptor,
                                                  Consolpp::Stream::FlushMode::Buffered,
                                                  true,
                                                  Consolpp::Terminal::Capabilities{}};

            // Act
            for ([[maybe_unused]]
                 const auto iteration : state) {
                for (int index = 0; index < lineCount; ++index) {
                    output
                        << Consolpp::Styling::StyledSpan{.style = Consolpp::Markup::Style::info, .text = "Deployed"}
                        << " file "
                        << index
                        << " of "
                        << lineCount
                        << '\n';
                }

                output.flush();
            }

            if (close(descriptor) != 0) {
                throw std::system_error{errno, std::generic_category()};
            }
        }
    } // namespace

    BENCHMARK(writePlain);
    BENCHMARK(writeFormattedMarkup);
    BENCHMARK(writeRuntimeMarkup);
    BENCHMARK(writeTyped);
} // namespace Consolpp::Tests::Benchmark::Styling
