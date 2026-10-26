#include "Consolpp/Parsing/HasValueInNextArgument.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"

#include <algorithm>
#include <cstddef>
#include <span>
#include <string_view>

namespace Consolpp::Parsing
{
    bool hasValueInNextArgument(const std::span<const Option> options, const Parsing::Token& token)
    {
        if (token.getKind() == Parsing::TokenKind::LongOption) {
            const auto found = std::ranges::find(options, token.getName(), &Option::name);

            return found != options.end() && found->value == OptionValue::Required && !token.getValue().has_value();
        }

        if (token.getKind() != Parsing::TokenKind::ShortOptions) {
            return false;
        }

        const std::string_view shortcuts = token.getName();

        for (std::size_t position = 0; position < shortcuts.size(); ++position) {
            const auto found = std::ranges::find(options, shortcuts.at(position), &Option::shortcut);

            if (found != options.end() && found->value != OptionValue::None) {
                return found->value == OptionValue::Required && position + 1 == shortcuts.size();
            }
        }

        return false;
    }
} // namespace Consolpp::Parsing
