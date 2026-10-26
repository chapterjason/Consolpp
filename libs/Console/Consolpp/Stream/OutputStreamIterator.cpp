#include "Consolpp/Stream/OutputStreamIterator.hpp"

#include "Consolpp/Stream/OutputStream.hpp"

namespace Consolpp::Stream
{
    OutputStreamIterator& OutputStreamIterator::operator=(const char character)
    {
        if (this->flushing) {
            this->writer->put(character);
        } else {
            this->writer->append(character);
        }

        return *this;
    }
} // namespace Consolpp::Stream
