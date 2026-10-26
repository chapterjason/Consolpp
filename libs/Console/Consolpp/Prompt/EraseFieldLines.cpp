#include "Consolpp/Prompt/EraseFieldLines.hpp"

#include "Consolpp/Stream/OutputStream.hpp"

#include <cstddef>

namespace Consolpp::Prompt
{
    void eraseFieldLines(Stream::OutputStream& output, const std::size_t rowsAboveCursor)
    {
        output.writeRaw("\r");

        if (rowsAboveCursor > 0) {
            output.writeRaw("\x1b[");
            output << rowsAboveCursor;
            output.writeRaw("A");
        }

        output.writeRaw("\x1b[0J");
    }
} // namespace Consolpp::Prompt
