# Input

`Consolpp::Stream::InputStream`, taken by a command as `run` parameter. It works the same in a terminal, a pipe or a file.

## Reading

```cpp
for (const std::string_view line : input.createLineRange()) {
    output.write("{}\n", line);
}
```

- `readLine()`, `readWord()`: `std::optional<std::string_view>`, empty at the end of the input.
- `input >> word`, `input >> number`, `input >> flag` for `std::string`, `std::string_view`, `char`, integers, floating points and `bool`.
- Returned views are overwritten by the next read; copy them to keep them.

## Prompts

```cpp
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"

const std::string name = input.ask(Consolpp::Prompt::TextField{Consolpp::Prompt::TextFieldOptions{
    .label = "Project name",
    .defaultValue = "app",
}});
```

| Field | Options | Answer |
|---|---|---|
| `TextField` | `label`, `defaultValue`, `echo`, `autocompletion` | `std::string` |
| `ChoiceField` | `label`, `choices`, `selection`, `defaultIndexes` | `std::vector<std::size_t>`, the chosen indexes |
| `ConfirmationField` | `label`, `defaultValue` | `bool` |

- `label` is markup.
- The texts and spans of the options must outlive the field: use `static constexpr` arrays.
- `echo`: `Echo::Visible`, `Echo::Masked` (`*` per grapheme, for passwords), `Echo::Hidden`.
- `autocompletion`: `Autocompletion::fromValues(values)`, or `Autocompletion{[](std::string_view text, std::vector<std::string>& completions) {...}}`. Tab takes the suggestion, Up and Down change it.
- `selection`: `Selection::Single` (one index, the highlighted choice) or `Selection::Multiple`.
- `ConfirmationField` without `defaultValue` needs an answer.

```cpp
static constexpr std::array<std::string_view, 3> environments{"development", "staging", "production"};
static constexpr std::array<std::size_t, 1> staging{1};

const std::vector<std::size_t> chosen = input.ask(Consolpp::Prompt::ChoiceField{Consolpp::Prompt::ChoiceFieldOptions{
    .label = "Environment",
    .choices = environments,
    .defaultIndexes = staging,
}});
const std::string_view environment = environments.at(chosen.front());
```

## Forms

Several fields asked together; Shift+Tab goes back to the previous field.

```cpp
#include "Consolpp/Prompt/Form.hpp"

const auto [name, region, backups] = input.ask(Consolpp::Prompt::Form{
    Consolpp::Prompt::TextField{Consolpp::Prompt::TextFieldOptions{.label = "Server name", .defaultValue = "web-1"}},
    Consolpp::Prompt::ChoiceField{Consolpp::Prompt::ChoiceFieldOptions{.label = "Region", .choices = regions}},
    Consolpp::Prompt::ConfirmationField{
        Consolpp::Prompt::ConfirmationFieldOptions{.label = "Daily backups?", .defaultValue = false}},
});
```

## Interaction

- Interactive when standard input is a terminal; `-n` / `--no-interaction` disables it. `isInteractive()` tells.
- Without interaction a field takes its default, or throws `Prompt::InteractionDisabledError` without one.
- Escape, Ctrl+C or the end of the input throw `Prompt::CancelledError`.
- Missing required arguments are asked for automatically, see [Commands](COMMANDS.md).

## Customizing

Set these in `main`, before any field is asked; they are not thread-safe.

- `InputStream::bindKey(key, action)`, `InputStream::setKeyBindings(bindings)`: the keys of the fields.
- `TextField::setQuestionTemplate(markup)`, likewise for `ChoiceField` and `ConfirmationField`: the question line, with placeholders such as `{label}` and `{default}`.
- Replace the `prompt-*` tags to restyle the fields, see [Markup](MARKUP.md).

## From a string

```cpp
Consolpp::Stream::InputStream input{"first line\nsecond line\n"};
```

The text must outlive the stream. Interaction is disabled. Useful in tests.
