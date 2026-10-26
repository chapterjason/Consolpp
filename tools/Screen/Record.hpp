#pragma once

#include "Consolpp/Event/Loop.hpp"

#include <span>
#include <string>

namespace Consolpp::Tools::Screen
{
    // Runs the command in a pseudo terminal of the size of the own one and passes the keys, the output and every
    // resize through, while it writes the output and the sizes to the file as entries. Returns the exit code of the
    // command. Throws std::system_error when the pseudo terminal or the file cannot be used.
    int record(std::span<const std::string> command, const std::string& path, Event::Loop& loop);
} // namespace Consolpp::Tools::Screen
