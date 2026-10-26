#include "Consolpp/Terminal/HasColorSupport.hpp"

#include "Consolpp/Environment/GetValue.hpp"

#include <algorithm>
#include <array>
#include <optional>
#include <string_view>

namespace Consolpp::Terminal
{
    bool hasColorSupport(const bool terminal)
    {
        constexpr std::array<std::string_view, 8> colorTermPrefixes{
            "screen",
            "xterm",
            "vt100",
            "vt220",
            "putty",
            "rxvt",
            "ansi",
            "linux",
        };
        constexpr std::array<std::string_view, 4> colorTermSuffixes{"-256", "-256color", "-256-bce", "-256color-bce"};
        const auto isNonEmpty = [](const char* name) {
            const std::optional<std::string_view> value = Environment::getValue(name);

            return value.has_value() && !value->empty();
        };

        if (isNonEmpty("NO_COLOR")) {
            return false;
        }

        if (isNonEmpty("FORCE_COLOR")) {
            return true;
        }

        if (!terminal) {
            return false;
        }

        if (Environment::getValue("TERM_PROGRAM") == "Hyper" || Environment::getValue("COLORTERM").has_value()) {
            return true;
        }

        const std::string_view term = Environment::getValue("TERM").value_or(std::string_view{});

        if (term == "dumb") {
            return false;
        }

        return std::ranges::any_of(colorTermPrefixes,
                                   [term](const std::string_view prefix) {
                                       return term.starts_with(prefix);
                                   })
               || std::ranges::any_of(colorTermSuffixes, [term](const std::string_view suffix) {
                      return term.ends_with(suffix);
                  });
    }
} // namespace Consolpp::Terminal
