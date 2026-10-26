#include "Consolpp/Stream/SourceBuffer.hpp"

#include "Consolpp/Stream/ReadDescriptor.hpp"

#include <cstddef>
#include <iterator>

namespace Consolpp::Stream
{
    SourceBuffer::SourceBuffer(const int descriptor)
        : descriptor{descriptor}
    {
    }

    SourceBuffer::int_type SourceBuffer::underflow()
    {
        if (this->gptr() < this->egptr()) {
            return traits_type::to_int_type(*this->gptr());
        }

        const std::size_t received = Stream::readDescriptor(this->descriptor, this->buffer);

        if (received == 0) {
            return traits_type::eof();
        }

        this->setg(this->buffer.data(),
                   this->buffer.data(),
                   std::next(this->buffer.data(), static_cast<std::ptrdiff_t>(received)));

        return traits_type::to_int_type(this->buffer.front());
    }
} // namespace Consolpp::Stream
