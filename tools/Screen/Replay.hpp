#pragma once

#include "Consolpp/Stream/OutputStream.hpp"

#include <string>

namespace Consolpp::Tools::Screen
{
    // Plays the recording back in an Emulator and writes the screen after every entry that changes it, optionally with
    // the scrollback. Throws std::system_error when the file cannot be read and std::invalid_argument for a recording
    // that does not start with a resize or holds a line of another form.
    void replay(const std::string& path, bool scrollback, Stream::OutputStream& output);
} // namespace Consolpp::Tools::Screen
