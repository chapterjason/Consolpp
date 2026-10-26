#include "Consolpp/Terminal/GetColumns.hpp"

#include "Consolpp/Terminal/GetEnvironmentSize.hpp"

#include <cstddef>

#include <sys/ioctl.h>

namespace Consolpp::Terminal
{
    std::size_t getColumns(const int descriptor)
    {
        constexpr std::size_t fallbackWidth = 80;
        winsize size{};

        if (ioctl(descriptor, TIOCGWINSZ, &size) == 0 && size.ws_col > 0) {
            return size.ws_col;
        }

        return Terminal::getEnvironmentSize("COLUMNS", fallbackWidth);
    }
} // namespace Consolpp::Terminal
