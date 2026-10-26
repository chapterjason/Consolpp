#pragma once

#include "Consolpp/Stream/OutputStream.hpp"

#include <cstddef>

namespace Consolpp::Prompt
{
    void eraseFieldLines(Stream::OutputStream& output, std::size_t rowsAboveCursor);
} // namespace Consolpp::Prompt
