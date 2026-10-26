#include "Consolpp/Terminal/DetectHyperlinks.hpp"

#include "Consolpp/Environment/GetValue.hpp"

#include <charconv>
#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <system_error>

namespace Consolpp::Terminal
{
    bool detectHyperlinks()
    {
        constexpr std::uint32_t lastKonsoleWithoutLinks = 201100;

        if (Environment::getValue("TERMINAL_EMULATOR") == "JetBrains-JediTerm"
            || Environment::getValue("IDEA_INITIAL_DIRECTORY").has_value()) {
            return false;
        }

        const std::optional<std::string_view> konsole = Environment::getValue("KONSOLE_VERSION");

        if (!konsole.has_value() || konsole->empty()) {
            return true;
        }

        const char* const first = std::to_address(konsole->begin());
        const char* const last = std::to_address(konsole->end());
        std::uint32_t version = 0;
        const std::from_chars_result result = std::from_chars(first, last, version);

        return result.ec == std::errc{} && result.ptr == last && version > lastKonsoleWithoutLinks;
    }
} // namespace Consolpp::Terminal
