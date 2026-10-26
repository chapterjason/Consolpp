#include "Consolpp/Parsing/TokenIterator.hpp"

#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string_view>

namespace Consolpp::Parsing
{
    Token TokenIterator::operator*() const
    {
        const char* const argument = *this->current;

        if (*argument != '-') {
            return Token{TokenKind::Positional, std::string_view{argument}, 0};
        }

        const char* const second = std::next(argument);

        if (*second == '\0') {
            return Token{TokenKind::Dash, std::string_view{argument, 1}, 0};
        }

        if (*second != '-') {
            return Token{TokenKind::ShortOptions, std::string_view{argument}, 1};
        }

        const char* const name = std::next(second);

        if (*name == '\0') {
            return Token{TokenKind::Separator, std::string_view{argument, 2}, 0};
        }

        const std::string_view option{name};
        const std::size_t equals = option.find('=');

        if (equals == std::string_view::npos) {
            return Token{TokenKind::LongOption, std::string_view{argument, 2 + option.size()}, 2};
        }

        return Token{TokenKind::LongOption,
                     std::string_view{argument, 2 + option.size()},
                     2,
                     static_cast<std::uint32_t>(equals)};
    }
} // namespace Consolpp::Parsing
