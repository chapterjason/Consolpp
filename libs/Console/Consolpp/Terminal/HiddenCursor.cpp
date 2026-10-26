#include "Consolpp/Terminal/HiddenCursor.hpp"

#include "Consolpp/Stream/OutputStream.hpp"

#include <system_error>

namespace Consolpp::Terminal
{
    HiddenCursor::HiddenCursor(Stream::OutputStream& output)
        : output{&output}
    {
        this->output->writeRaw("\x1b[?25l");
        this->output->flush();
    }

    HiddenCursor::~HiddenCursor()
    {
        try {
            this->output->writeRaw("\x1b[?25h");
            this->output->flush();
            // A destructor cannot throw; like the stream at its end, a failed write is dropped.
            // NOLINTNEXTLINE(bugprone-empty-catch)
        } catch (const std::system_error&) {
        }
    }
} // namespace Consolpp::Terminal
