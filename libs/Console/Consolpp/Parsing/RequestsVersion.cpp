#include "Consolpp/Parsing/RequestsVersion.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/HasValueInNextArgument.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <span>
#include <string_view>

namespace Consolpp::Parsing
{
    bool requestsVersion(const std::span<const Option> options, const TokenRange tokens)
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

            if (kind == TokenKind::LongOption && token.getName() == "version") {
                return true;
            }

            valueNext = Parsing::hasValueInNextArgument(options, token);
        }

        return false;
    }
} // namespace Consolpp::Parsing
