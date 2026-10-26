# Completion

Every program completes its commands, options and values in Bash. The user loads it once, e.g. in `.bashrc`:

```sh
eval "$(app completion bash)"
```

## Values

Each option or argument takes one of these, not both:

- `suggestedValues`: fixed values.

```cpp
inline constexpr std::array<std::string_view, 3> formats{"txt", "json", "md"};

Consolpp::Option{
    .name = "format",
    .value = Consolpp::OptionValue::Required,
    .suggestedValues = formats,
},
```

- `completion`: a mode.

| `ValueCompletion` | Completes |
|---|---|
| `None` | Nothing. Default. |
| `File` | Files; directories only to descend into. |
| `Directory` | Directories. |
| `Path` | Files and directories. |
| `Computed` | The values the command's `complete` returns. |

Flags have no value, so they take neither.

## complete

A command with a `Computed` option or argument must have `complete`, or it does not compile.

```cpp
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Completion/ValueTarget.hpp"

static std::vector<std::string> complete(const Consolpp::Completion::Request& request)
{
    if (request.target == Consolpp::Completion::ValueTarget::Option && request.name == "environment") {
        return std::vector<std::string>{"development", "staging", "production"};
    }

    return listBranches();
}
```

- `request.target`: `ValueTarget::Option` or `ValueTarget::Argument`.
- `request.name`: the name of the option or argument.
- `request.value`: what is typed so far.
- After the request it may take `const Consolpp::CommandList&` and `const Consolpp::Program&`.
- Static or member, never overloaded.
- It cannot write output: the output carries the answer to the shell.
