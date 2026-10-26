#include "Consolpp/Terminal/DetectDecoration.hpp"

#include "Consolpp/Terminal/HasColorSupport.hpp"

#include <unistd.h>

namespace Consolpp::Terminal
{
    bool detectDecoration(const int descriptor)
    {
        return Terminal::hasColorSupport(isatty(descriptor) == 1);
    }
} // namespace Consolpp::Terminal
