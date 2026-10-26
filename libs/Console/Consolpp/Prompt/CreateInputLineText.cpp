#include "Consolpp/Prompt/CreateInputLineText.hpp"

#include "Consolpp/Prompt/Echo.hpp"
#include "Consolpp/Prompt/InputLineText.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Text/Grapheme/FindEnd.hpp"
#include "Consolpp/Text/ReplaceControlCharacters.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    namespace
    {
        std::size_t countGraphemes(const std::string_view text)
        {
            std::size_t count = 0;

            for (std::size_t position = 0; position < text.size(); position = Text::Grapheme::findEnd(text, position)) {
                ++count;
            }

            return count;
        }
    } // namespace

    InputLineText createInputLineText(const TextField& field)
    {
        const std::string_view text = field.getText();

        switch (field.getOptions().echo) {
            case Echo::Masked:
                return InputLineText{
                    .text = std::string(countGraphemes(text), '*'),
                    .cursor = countGraphemes(text.substr(0, field.getCursor())),
                };
            case Echo::Hidden:
                return InputLineText{};
            case Echo::Visible:
                break;
        }

        return InputLineText{
            .text = Text::replaceControlCharacters(text),
            .cursor = Text::replaceControlCharacters(text.substr(0, field.getCursor())).size(),
        };
    }
} // namespace Consolpp::Prompt
