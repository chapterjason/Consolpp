#include "Consolpp/Terminal/DetectCapabilities.hpp"

#include "Consolpp/Terminal/Capabilities.hpp"
#include "Consolpp/Terminal/DetectColorMode.hpp"
#include "Consolpp/Terminal/DetectHyperlinks.hpp"

namespace Consolpp::Terminal
{
    Capabilities detectCapabilities()
    {
        return Capabilities{
            .colorMode = detectColorMode(),
            .hyperlinks = detectHyperlinks(),
        };
    }
} // namespace Consolpp::Terminal
