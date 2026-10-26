#include "Consolpp/Terminal/SynchronizedUpdate.hpp"

#include "Consolpp/Stream/OutputStream.hpp"

#include <system_error>

namespace Consolpp::Terminal
{
    SynchronizedUpdate::SynchronizedUpdate(Stream::OutputStream& output)
        : output{&output}
    {
        this->output->writeRaw("\x1b[?2026h");
    }

    SynchronizedUpdate::~SynchronizedUpdate()
    {
        try {
            this->output->writeRaw("\x1b[?2026l");
            // A destructor cannot throw; like the stream at its end, a failed write is dropped.
            // NOLINTNEXTLINE(bugprone-empty-catch)
        } catch (const std::system_error&) {
        }
    }
} // namespace Consolpp::Terminal
