#pragma once

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/ParsedMarkupSlice.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStreamIterator.hpp"
#include "Consolpp/Stream/PlainArgument.hpp"
#include "Consolpp/Stream/Verbosity.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"
#include "Consolpp/Styling/StyledSpan.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"
#include "Consolpp/Terminal/DetectCapabilities.hpp"

#include <array>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <format>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Consolpp::Stream
{
    class OutputStream
    {
        private:
            friend class OutputStreamIterator;

            static constexpr std::size_t capacity = 65536;
            static constexpr std::size_t numberCapacity = 64;

            std::array<char, capacity> buffer{};
            std::size_t used{0};
            int descriptor;
            // Written to instead of the descriptor when set.
            std::string* target{nullptr};
            FlushMode mode;
            Terminal::Capabilities capabilities;
            bool decorated;
            bool markup{true};
            Verbosity verbosity{Verbosity::Normal};
            // Reused for markup formats, so formatting allocates only until it has grown once.
            std::string formatted;

            void writeBytes(std::span<const char> bytes) const;

            // Appending leaves the flush to the public operation, so it reaches the descriptor in one piece.
            void append(const std::string_view text);

            void append(const char character)
            {
                if (this->used == this->buffer.size()) {
                    this->flush();
                }

                *std::next(this->buffer.begin(), static_cast<std::ptrdiff_t>(this->used)) = character;
                ++this->used;

                if (this->mode == FlushMode::Line && character == '\n') {
                    this->flush();
                }
            }

            void appendNumber(const auto number)
            {
                std::array<char, numberCapacity> digits{};
                const std::to_chars_result result = std::to_chars(digits.data(),
                                                                  std::next(digits.data(), digits.size()),
                                                                  number);

                this->append(std::string_view{digits.data(), result.ptr});
            }

            // While markup is enabled the text is read as markup; invalid markup throws before anything is written.
            void appendMarkup(const std::string_view text);

            void flushIfImmediate()
            {
                if (this->mode == FlushMode::Immediate) {
                    this->flush();
                }
            }

            [[nodiscard]]
            OutputStreamIterator createAppendIterator()
            {
                return OutputStreamIterator{*this, false};
            }

        public:
            // Decorates when the descriptor is a terminal and the environment allows colors.
            OutputStream(const int descriptor, const FlushMode mode);

            OutputStream(const int descriptor,
                         const FlushMode mode,
                         const bool decorated,
                         const Terminal::Capabilities capabilities = Terminal::detectCapabilities());

            // Writes into the target, which must outlive the stream, at once instead of to a descriptor, so the target
            // always holds everything written. It is no terminal, and decorated only when asked.
            explicit OutputStream(std::string& target,
                                  const bool decorated = false,
                                  const Terminal::Capabilities capabilities = Terminal::Capabilities{});

            OutputStream(const OutputStream&) = delete;
            OutputStream(OutputStream&&) = delete;
            OutputStream& operator=(const OutputStream&) = delete;
            OutputStream& operator=(OutputStream&&) = delete;

            ~OutputStream();

            // Formats like std::format, at the level Normal. While markup is enabled, the format is read as markup and
            // the arguments are inserted as plain text; invalid markup throws before anything is written.
            template <typename... Arguments>
            OutputStream& write(const std::format_string<PlainArgument<Arguments>...> format,
                                const Arguments&... arguments)
            {
                return this->write(Verbosity::Normal, format, arguments...);
            }

            // Written only while the level is at most the stream's verbosity.
            template <typename... Arguments>
            OutputStream& write(const Verbosity level,
                                const std::format_string<PlainArgument<Arguments>...> format,
                                const Arguments&... arguments)
            {
                if (!this->isShown(level)) {
                    return *this;
                }

                if (!this->markup || format.get().find_first_of("<&") == std::string_view::npos) {
                    std::vformat_to(this->createAppendIterator(), format.get(), std::make_format_args(arguments...));
                    this->flushIfImmediate();

                    return *this;
                }

                const std::tuple<const PlainArgument<Arguments>...> plain{PlainArgument<Arguments>{&arguments}...};

                this->formatted.clear();
                std::apply(
                    [this, &format](const auto&... values) {
                        std::vformat_to(std::back_inserter(this->formatted),
                                        format.get(),
                                        std::make_format_args(values...));
                    },
                    plain);
                this->appendMarkup(this->formatted);
                this->flushIfImmediate();

                return *this;
            }

            // Writes the text exactly as it is, markup or not.
            OutputStream& writeRaw(const std::string_view text);

            void flush();

            void put(const char character)
            {
                if (!this->isShown(Verbosity::Normal)) {
                    return;
                }

                this->append(character);
                this->flushIfImmediate();
            }

            [[nodiscard]]
            OutputStreamIterator createIterator()
            {
                return OutputStreamIterator{*this, true};
            }

            void setFlushMode(const FlushMode mode)
            {
                this->mode = mode;
            }

            [[nodiscard]]
            FlushMode getFlushMode() const
            {
                return this->mode;
            }

            // Everything written without a level counts as Normal.
            void setVerbosity(const Verbosity verbosity)
            {
                this->verbosity = verbosity;
            }

            [[nodiscard]]
            Verbosity getVerbosity() const
            {
                return this->verbosity;
            }

            [[nodiscard]]
            bool isShown(const Verbosity level) const
            {
                return level <= this->verbosity;
            }

            [[nodiscard]]
            bool isQuiet() const
            {
                return this->verbosity <= Verbosity::Quiet;
            }

            [[nodiscard]]
            bool isVerbose() const
            {
                return this->verbosity >= Verbosity::Verbose;
            }

            [[nodiscard]]
            bool isVeryVerbose() const
            {
                return this->verbosity >= Verbosity::VeryVerbose;
            }

            [[nodiscard]]
            bool isDebug() const
            {
                return this->verbosity >= Verbosity::Debug;
            }

            [[nodiscard]]
            bool isDecorated() const
            {
                return this->decorated;
            }

            void enableMarkup()
            {
                this->markup = true;
            }

            void disableMarkup()
            {
                this->markup = false;
            }

            [[nodiscard]]
            bool isMarkupEnabled() const
            {
                return this->markup;
            }

            [[nodiscard]]
            bool isTerminal() const;

            [[nodiscard]]
            std::size_t getColumns() const;

            // The width elements are written at at most: on a terminal one column less than it has, as terminals join
            // a line that fills a row with the next one when they reflow it; elsewhere unlimited.
            [[nodiscard]]
            std::size_t getAvailableWidth() const;

            [[nodiscard]]
            std::size_t getRows() const;

            OutputStream& operator<<(const std::string_view text)
            {
                if (!this->isShown(Verbosity::Normal)) {
                    return *this;
                }

                this->appendMarkup(text);
                this->flushIfImmediate();

                return *this;
            }

            OutputStream& operator<<(const char character)
            {
                this->put(character);

                return *this;
            }

            OutputStream& operator<<(const Markup::ParsedMarkup<Markup::ElementStyle>& markup);

            OutputStream& operator<<(const Markup::ParsedMarkupSlice<Markup::ElementStyle>& slice);

            OutputStream& operator<<(const Styling::StyledSpan& span);

            // Every line followed by a line break; the lines are markup even while markup is disabled.
            OutputStream& operator<<(const Layout::Canvas& canvas);

            // At its natural width, at most the available width.
            OutputStream& operator<<(const Layout::Element& element);

            template <std::same_as<bool> Value>
            OutputStream& operator<<(const Value value)
            {
                return this->writeRaw(value ? "true" : "false");
            }

            template <typename Value>
                requires((std::integral<Value> || std::floating_point<Value>)
                         && !std::same_as<Value, bool>
                         && !std::same_as<Value, char>)
            OutputStream& operator<<(const Value value)
            {
                if (!this->isShown(Verbosity::Normal)) {
                    return *this;
                }

                this->appendNumber(value);
                this->flushIfImmediate();

                return *this;
            }
    };
} // namespace Consolpp::Stream
