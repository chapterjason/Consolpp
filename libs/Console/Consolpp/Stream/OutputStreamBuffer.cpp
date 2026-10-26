#include "Consolpp/Stream/OutputStreamBuffer.hpp"

#include "Consolpp/Stream/OutputStream.hpp"

#include <cstddef>
#include <ios>
#include <string_view>

namespace Consolpp::Stream
{
    OutputStreamBuffer::OutputStreamBuffer(OutputStream& writer)
        : writer{&writer}
    {
    }

    OutputStreamBuffer::int_type OutputStreamBuffer::overflow(const int_type character)
    {
        if (traits_type::eq_int_type(character, traits_type::eof())) {
            return traits_type::not_eof(character);
        }

        *this->writer << traits_type::to_char_type(character);

        return character;
    }

    std::streamsize OutputStreamBuffer::xsputn(const char_type* text, const std::streamsize count)
    {
        this->writer->writeRaw(std::string_view{text, static_cast<std::size_t>(count)});

        return count;
    }

    int OutputStreamBuffer::sync()
    {
        this->writer->flush();

        return 0;
    }
} // namespace Consolpp::Stream
