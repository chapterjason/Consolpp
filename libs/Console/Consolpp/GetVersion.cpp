#include "Consolpp/GetVersion.hpp"

#include "Consolpp/Version.hpp"

#include <string_view>

namespace Consolpp
{
    std::string_view getVersion()
    {
        return version;
    }
} // namespace Consolpp
