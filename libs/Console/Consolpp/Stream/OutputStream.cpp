#include "Consolpp/Stream/OutputStream.hpp"

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/ParsedMarkupSlice.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/Verbosity.hpp"
#include "Consolpp/Styling/EncodeMarkup.hpp"
#include "Consolpp/Styling/EncodeParsedMarkupSlice.hpp"
#include "Consolpp/Styling/EncodeStyleEnd.hpp"
#include "Consolpp/Styling/EncodeStyleStart.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"
#include "Consolpp/Styling/StyledSpan.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"
#include "Consolpp/Terminal/DetectDecoration.hpp"
#include "Consolpp/Terminal/GetColumns.hpp"
#include "Consolpp/Terminal/GetRows.hpp"

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <limits>
#include <new>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

#include <sys/types.h> // IWYU pragma: keep
#include <unistd.h>

namespace Consolpp::Stream
{
    OutputStream::OutputStream(const int descriptor, const FlushMode mode)
        : OutputStream{descriptor, mode, Terminal::detectDecoration(descriptor)}
    {
    }

    OutputStream::OutputStream(const int descriptor,
                               const FlushMode mode,
                               const bool decorated,
                               const Terminal::Capabilities capabilities)
        : descriptor{descriptor},
          mode{mode},
          capabilities{capabilities},
          decorated{decorated}
    {
    }

    OutputStream::OutputStream(std::string& target, const bool decorated, const Terminal::Capabilities capabilities)
        : descriptor{-1},
          target{&target},
          mode{FlushMode::Immediate},
          capabilities{capabilities},
          decorated{decorated}
    {
    }

    OutputStream::~OutputStream()
    {
        try {
            this->flush();
            // A destructor cannot throw; like stdio at exit, a failed last write is dropped.
            // NOLINTNEXTLINE(bugprone-empty-catch)
        } catch (const std::system_error&) {
            // A string target that cannot grow loses its last write the same way.
            // NOLINTNEXTLINE(bugprone-empty-catch)
        } catch (const std::bad_alloc&) {
            // NOLINTNEXTLINE(bugprone-empty-catch)
        } catch (const std::length_error&) {
        }
    }

    void OutputStream::writeBytes(std::span<const char> bytes) const
    {
        if (this->target != nullptr) {
            this->target->append(bytes.data(), bytes.size());

            return;
        }

        while (!bytes.empty()) {
            const ssize_t written = ::write(this->descriptor, bytes.data(), bytes.size());

            if (written < 0 && errno == EINTR) {
                continue;
            }

            if (written < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            bytes = bytes.subspan(static_cast<std::size_t>(written));
        }
    }

    bool OutputStream::isTerminal() const
    {
        return this->target == nullptr && isatty(this->descriptor) == 1;
    }

    std::size_t OutputStream::getColumns() const
    {
        return Terminal::getColumns(this->descriptor);
    }

    std::size_t OutputStream::getAvailableWidth() const
    {
        if (!this->isTerminal()) {
            return std::numeric_limits<std::size_t>::max();
        }

        const std::size_t columns = this->getColumns();

        return columns > 1 ? columns - 1 : columns;
    }

    std::size_t OutputStream::getRows() const
    {
        return Terminal::getRows(this->descriptor);
    }

    void OutputStream::append(const std::string_view text)
    {
        if (text.empty()) {
            return;
        }

        if (text.size() > this->buffer.size() - this->used) {
            this->flush();
        }

        if (text.size() >= this->buffer.size()) {
            this->writeBytes(text);

            return;
        }

        std::memcpy(std::next(this->buffer.data(), static_cast<std::ptrdiff_t>(this->used)), text.data(), text.size());

        this->used += text.size();

        if (this->mode == FlushMode::Line && text.find('\n') != std::string_view::npos) {
            this->flush();
        }
    }

    void OutputStream::appendMarkup(const std::string_view text)
    {
        // Without "<" and "&" there is neither a tag nor an entity, so the text stays as it is.
        if (!this->markup || text.find_first_of("<&") == std::string_view::npos) {
            this->append(text);

            return;
        }

        if (this->decorated) {
            Styling::encodeMarkup(text, this->capabilities, this->createAppendIterator());
        } else {
            Styling::stripMarkup(text, this->createAppendIterator());
        }
    }

    OutputStream& OutputStream::writeRaw(const std::string_view text)
    {
        if (!this->isShown(Verbosity::Normal)) {
            return *this;
        }

        this->append(text);
        this->flushIfImmediate();

        return *this;
    }

    void OutputStream::flush()
    {
        const std::span<const char> pending{this->buffer.data(), this->used};

        this->used = 0;
        this->writeBytes(pending);
    }

    OutputStream& OutputStream::operator<<(const Markup::ParsedMarkup<Markup::ElementStyle>& markup)
    {
        return *this << markup.getSlice(0, markup.getText().size());
    }

    OutputStream& OutputStream::operator<<(const Layout::Canvas& canvas)
    {
        if (!this->isShown(Verbosity::Normal)) {
            return *this;
        }

        for (std::size_t index = 0; index < canvas.getCount(); ++index) {
            const std::string_view line = canvas.getLine(index);

            if (this->decorated) {
                Styling::encodeMarkup(line, this->capabilities, this->createAppendIterator());
            } else {
                Styling::stripMarkup(line, this->createAppendIterator());
            }

            this->append('\n');
        }

        this->flushIfImmediate();

        return *this;
    }

    OutputStream& OutputStream::operator<<(const Layout::Element& element)
    {
        Layout::Canvas canvas;

        if (!this->isShown(Verbosity::Normal)) {
            return *this;
        }

        element.render(std::min(element.measure().natural, this->getAvailableWidth()), canvas);

        return *this << canvas;
    }

    OutputStream& OutputStream::operator<<(const Markup::ParsedMarkupSlice<Markup::ElementStyle>& slice)
    {
        if (!this->isShown(Verbosity::Normal)) {
            return *this;
        }

        if (this->decorated) {
            Styling::encodeParsedMarkupSlice(slice, this->capabilities, this->createAppendIterator());
        } else {
            this->append(slice.markup->getText().substr(slice.start, slice.end - slice.start));
        }

        this->flushIfImmediate();

        return *this;
    }

    OutputStream& OutputStream::operator<<(const Styling::StyledSpan& span)
    {
        if (!this->isShown(Verbosity::Normal)) {
            return *this;
        }

        if (this->decorated) {
            Styling::encodeStyleStart(span.style, this->capabilities, this->createAppendIterator());
        }

        this->append(span.text);

        if (this->decorated) {
            Styling::encodeStyleEnd(span.style, this->createAppendIterator());
        }

        this->flushIfImmediate();

        return *this;
    }
} // namespace Consolpp::Stream
