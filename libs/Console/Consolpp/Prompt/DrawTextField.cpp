#include "Consolpp/Prompt/DrawTextField.hpp"

#include "Consolpp/Prompt/DrawLines.hpp"
#include "Consolpp/Prompt/FormatTextField.hpp"
#include "Consolpp/Prompt/FormattedTextField.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <utility>

namespace Consolpp::Prompt
{
    std::size_t drawTextField(const TextField& field, Stream::OutputStream& output, const std::size_t rowsAboveCursor)
    {
        FormattedTextField formatted = Prompt::formatTextField(field, std::max<std::size_t>(output.getColumns(), 1));
        const std::array<std::string, 2> drawn{std::move(formatted.question), std::move(formatted.input)};

        return Prompt::drawLines(drawn, 1, formatted.cursorColumn, output, rowsAboveCursor);
    }
} // namespace Consolpp::Prompt
