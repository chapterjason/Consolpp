#include "Consolpp/Terminal/BracketedPaste.hpp"

#include "Consolpp/Stream/OutputStream.hpp"

#include <system_error>

namespace Consolpp::Terminal
{
    BracketedPaste::BracketedPaste(Stream::OutputStream& output)
        : output{&output}
    {
        this->output->writeRaw("\x1b[?2004h");
        this->output->flush();
    }

    BracketedPaste::~BracketedPaste()
    {
        try {
            this->output->writeRaw("\x1b[?2004l");
            this->output->flush();
            // A destructor cannot throw; like the stream at its end, a failed write is dropped.
            // NOLINTNEXTLINE(bugprone-empty-catch)
        } catch (const std::system_error&) {
        }
    }
} // namespace Consolpp::Terminal
