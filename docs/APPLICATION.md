# Application

## main

```cpp
#include "Consolpp/CommandManager.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <exception>

#include <unistd.h>

int main([[maybe_unused]] const int argc, char** const argv)
{
    Consolpp::Event::Loop loop;
    Consolpp::Stream::OutputStream standardOutput{STDOUT_FILENO, Consolpp::Stream::FlushMode::Line};
    Consolpp::Stream::ErrorOutputStream standardErrorOutput{STDERR_FILENO, Consolpp::Stream::FlushMode::Immediate};
    Consolpp::Stream::InputStream standardInput{STDIN_FILENO, standardOutput, loop};
    GreetCommand greetCommand;
    DeployCommand deployCommand;
    const auto commandManager = Consolpp::CommandManager{standardInput, standardOutput, standardErrorOutput, loop}
                                    .addCommand(greetCommand)
                                    .addCommand(deployCommand)
                                    .setName("app")
                                    .setVersion("1.0.0");

    try {
        return commandManager.run(argv);
    } catch (const std::exception& exception) {
        standardErrorOutput.write("Error: <error>{}</error>\n", exception.what());

        return 1;
    }
}
```

## CommandManager

- Every setter returns a new manager, so chain them and keep the result.
- `addCommand(command)` holds the command by reference: it must outlive the manager.
- `setName(name)`: the program name in help and man pages. Without one, the file name of `argv[0]`.
- `setVersion(version)`: shown by `--version` and in the man page.
- `setDefaultCommand<Type>()`: runs when argv names no command.
- `setDefaultCommand<Type>(true)`: the program has this single command. All of argv belongs to it: `app Alice` instead of `app greet Alice`.
- `run(argv)` returns the exit code of the command.
- Two commands with the same name, or a name of a built-in command, do not compile.

## Built-in commands

| Command | Does |
|---|---|
| `list [namespace] [--format txt\|json\|md]` | Lists the commands. Runs when argv is empty and there is no default command. |
| `help [command] [--format txt\|json\|md]` | Describes a command. |
| `man [--file path]` | Generates the man page. |
| `completion bash` | Prints the completion script, see [Completion](COMPLETION.md). |

## Global options

Every command has these. A command cannot declare their names or shortcuts.

| Option | Does |
|---|---|
| `-h`, `--help` | Help for the command instead of running it. |
| `--version` | The program name and version instead of running the command. |
| `-q`, `--quiet` | Only errors. |
| `--silent` | No output at all. |
| `-v`, `-vv`, `-vvv`, `--verbose` | Verbose, very verbose, debug. |
| `-n`, `--no-interaction` | No prompts: fields take their default. |

## Errors

Errors are thrown, never returned. Catch `std::exception` around `run()`, as in `main` above.

- `Consolpp::CommandError`: unknown command or namespace. `createSuggestionRange()` gives similar names.
- `Consolpp::Parsing::ParseError`: unknown option, missing value, missing argument, too many arguments.
- `Consolpp::Prompt::CancelledError`: the user cancelled a prompt.
- `Consolpp::Prompt::InteractionDisabledError`: a field without default was asked with interaction disabled.
- Throw from your command's `run` to fail it; return a non-zero code for an exit code without a message.
