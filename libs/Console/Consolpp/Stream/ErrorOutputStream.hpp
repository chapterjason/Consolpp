#pragma once

#include "Consolpp/Stream/OutputStream.hpp"

namespace Consolpp::Stream
{
    class ErrorOutputStream : public OutputStream
    {
        public:
            using OutputStream::OutputStream;
    };
} // namespace Consolpp::Stream
