#include "Consolpp/Stream/ReadDescriptor.hpp"

#include <cerrno>
#include <cstddef>
#include <span>
#include <system_error>

#include <sys/types.h> // IWYU pragma: keep
#include <unistd.h>

namespace Consolpp::Stream
{
    std::size_t readDescriptor(const int descriptor, const std::span<char> target)
    {
        while (true) {
            const ssize_t received = read(descriptor, target.data(), target.size());

            if (received < 0 && errno == EINTR) {
                continue;
            }

            if (received < 0) {
                throw std::system_error{errno, std::generic_category()};
            }

            return static_cast<std::size_t>(received);
        }
    }
} // namespace Consolpp::Stream
