#include "Consolpp/Terminal/RawMode.hpp"

#include <cerrno>
#include <system_error>

#include <termios.h>
#include <unistd.h>

namespace Consolpp::Terminal
{
    RawMode::RawMode(const int descriptor)
        : descriptor{descriptor}
    {
        termios settings{};

        if (isatty(descriptor) != 1) {
            return;
        }

        if (tcgetattr(descriptor, &settings) != 0) {
            throw std::system_error{errno, std::generic_category()};
        }

        this->previous = settings;
        settings.c_iflag &= ~static_cast<tcflag_t>(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        settings.c_cflag |= static_cast<tcflag_t>(CS8);
        settings.c_lflag &= ~static_cast<tcflag_t>(ECHO | ICANON | IEXTEN | ISIG);
        settings.c_cc[VMIN] = 1;
        settings.c_cc[VTIME] = 0;

        if (tcsetattr(descriptor, TCSANOW, &settings) != 0) {
            throw std::system_error{errno, std::generic_category()};
        }
    }

    RawMode::~RawMode()
    {
        if (this->previous.has_value()) {
            // A destructor cannot throw; a terminal that refuses its old settings has nothing better to fall back to.
            static_cast<void>(tcsetattr(this->descriptor, TCSANOW, &*this->previous));
        }
    }
} // namespace Consolpp::Terminal
