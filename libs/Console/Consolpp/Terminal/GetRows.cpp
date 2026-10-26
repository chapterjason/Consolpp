#include "Consolpp/Terminal/GetRows.hpp"

#include "Consolpp/Terminal/GetEnvironmentSize.hpp"

#include <cstddef>

#include <sys/ioctl.h>

namespace Consolpp::Terminal
{
    std::size_t getRows(const int descriptor)
    {
        constexpr std::size_t fallbackHeight = 24;
        winsize size{};

        if (ioctl(descriptor, TIOCGWINSZ, &size) == 0 && size.ws_row > 0) {
            return size.ws_row;
        }

        return Terminal::getEnvironmentSize("LINES", fallbackHeight);
    }
} // namespace Consolpp::Terminal
