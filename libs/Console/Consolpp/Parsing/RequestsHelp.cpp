#include "Consolpp/Parsing/RequestsHelp.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/HasValueInNextArgument.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <algorithm>
#include <span>
#include <string_view>

namespace Consolpp::Parsing
{
    namespace
    {
        bool hasHelpShortcut(const std::span<const Option> options, const std::string_view shortcuts)
        {
            for (const char shortcut : shortcuts) {
                if (shortcut == 'h') {
                    return true;
                }

                const auto found = std::ranges::find(options, shortcut, &Option::shortcut);

                if (found != options.end() && found->value != OptionValue::None) {
                    return false;
                }
            }

            return false;
        }
    } // namespace

    bool requestsHelp(const std::span<const Option> options, const TokenRange tokens)
    {
        bool valueNext = false;

        for (const Token token : tokens) {
            const TokenKind kind = token.getKind();

            if (valueNext) {
                valueNext = false;

                continue;
            }

            if (kind == TokenKind::Separator) {
                return false;
            }

            if ((kind == TokenKind::LongOption && token.getName() == "help")
                || (kind == TokenKind::ShortOptions && hasHelpShortcut(options, token.getName()))) {
                return true;
            }

            valueNext = Parsing::hasValueInNextArgument(options, token);
        }

        return false;
    }
} // namespace Consolpp::Parsing
