# Commands

A command is a type with a `static constexpr Consolpp::Definition definition` and a `run` function. Definitions are validated at compile time: a broken definition does not compile.

```cpp
#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <string_view>

class GreetCommand
{
    public:
        static constexpr Consolpp::Definition definition{
            .name = "greet",
            .description = "Greets someone",
            .signature =
                Consolpp::Signature{
                    std::array{
                        Consolpp::Option{.name = "shout", .shortcut = 's', .description = "Greets loudly"},
                        Consolpp::Option{
                            .name = "greeting",
                            .value = Consolpp::OptionValue::Required,
                            .description = "The greeting",
                            .defaultValue = "Hello",
                        },
                    },
                    std::array{Consolpp::Argument{.name = "name", .description = "Who to greet"}},
                },
        };

        static int run(const Consolpp::Parsing::Parameters<definition>& parameters,
                       Consolpp::Stream::OutputStream& output)
        {
            const std::string_view greeting = parameters.getOption<"greeting">();
            const std::string_view name = parameters.getArgument<"name">();

            if (parameters.getOption<"shout">().value_or(false)) {
                output.write("<info>{}, {}!</info>\n", greeting, name);

                return 0;
            }

            output.write("{}, <info>{}</info>.\n", greeting, name);

            return 0;
        }
};
```

A command without options or arguments:

```cpp
.signature = Consolpp::Signature{std::array<Consolpp::Option, 0>{}, std::array<Consolpp::Argument, 0>{}},
```

## Definition

| Field | Meaning |
|---|---|
| `name` | Required. `cache:clear` puts the command into the namespace `cache`. |
| `description` | One line in `list`. |
| `aliases` | `std::array<std::string_view, N>{...}`, other names that run the command. |
| `help` | Longer text in `help`. |
| `usages` | `std::array<std::string_view, N>{...}`, example invocations without the program name. |
| `hidden` | Left out of `list`. |
| `signature` | `Consolpp::Signature{options, arguments}`. |

`description` and `help`, also the `description` of options and arguments, are [markup](MARKUP.md): styled in the terminal, stripped for Markdown, man pages and JSON. A literal `<` or `&` is written `&lt;` or `&amp;`. Malformed markup does not compile; a tag that is not registered throws when the text is shown.

## Option

| Field | Meaning |
|---|---|
| `name` | `--name`. No leading dash, no `=`. |
| `shortcut` | `-n`. |
| `value` | `OptionValue::None` (flag, default), `Required` (`--name value`), `Optional` (`--name` or `--name=value`). |
| `repeatable` | Given several times. Needs a value. |
| `defaultValue` | When absent. Single value only. |
| `defaultValues` | When absent. Repeatable only. |
| `defaultEnabled` | Flag state when absent. Flags can always be negated with `--no-name`. |
| `suggestedValues`, `completion` | See [Completion](COMPLETION.md). One of them, not both. |

## Argument

| Field | Meaning |
|---|---|
| `name` | The key for `getArgument<"name">()`. |
| `required` | Default `true`. A required argument cannot follow an optional one. |
| `variadic` | Takes all remaining values. Last argument only. |
| `defaultValue` | Optional, non-variadic only. |
| `defaultValues` | Optional variadic only. |
| `suggestedValues`, `completion` | See [Completion](COMPLETION.md). |

A missing required argument is asked for when the input is interactive, and throws `ParseError` otherwise.

## Reading parameters

`getOption<"name">()` and `getArgument<"name">()`. An unknown name does not compile. The type depends on the declaration:

| Declaration | Type |
|---|---|
| Flag | `std::optional<bool>`: absent, `true` or `false` for `--no-name` |
| Flag with `defaultEnabled` | `bool` |
| `Required` value | `std::optional<std::string_view>` |
| `Required` value with `defaultValue` | `std::string_view` |
| `Optional` value | `Parsing::OptionalValue{given, value}` |
| `Optional` value with `defaultValue` | `std::optional<std::string_view>`: the default when absent, empty when given without value |
| Repeatable | Range of `std::string_view` (`Required`) or `std::optional<std::string_view>` (`Optional`) |
| Required argument | `std::string_view` |
| Optional argument | `std::optional<std::string_view>` |
| Optional argument with `defaultValue` | `std::string_view` |
| Variadic argument | Range of `std::string_view` |

Values are views into argv; copy them to keep them.

## run

- Returns `int`, the exit code. Static or member, never overloaded or a template.
- Takes any of these parameters, in any order, and nothing else:

| Parameter | Is |
|---|---|
| `const Consolpp::Parsing::Parameters<definition>&` | The parsed options and arguments. |
| `Consolpp::Stream::InputStream&` | Standard input, see [Input](INPUT.md). |
| `Consolpp::Stream::OutputStream&` | Standard output, see [Output](OUTPUT.md). |
| `Consolpp::Stream::ErrorOutputStream&` | Standard error. |
| `Consolpp::Event::Loop&` | See [Events](EVENTS.md). |
| `const Consolpp::CommandList&` | All commands, for commands that describe the others. |
| `const Consolpp::Program&` | Program name and version. |

## interact

Optional. `void interact(...)` takes the same parameters as `run`, with `Consolpp::Parsing::Parameters<definition>&` non-const. It runs only when the input is interactive, before missing arguments are asked for, so it can ask for them itself:

```cpp
static void interact(Consolpp::Parsing::Parameters<definition>& parameters, Consolpp::Stream::InputStream& input)
{
    if (parameters.isArgumentMissing(0)) {
        parameters.setArgument<"name">(input.ask(Consolpp::Prompt::TextField{
            Consolpp::Prompt::TextFieldOptions{.label = "Who should be greeted?"}}));
    }
}
```

`setArgument` copies the value.
