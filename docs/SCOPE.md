# Scope

Consolpp is a C++ library for building command-line applications. This file lists what belongs to it; anything not listed is out of scope until it is added here.

## Boundary

Consolpp provides the building blocks. The application itself, which puts them together, is built by the consumer and is not part of Consolpp. Each feature here is a part an application uses, never a decision only the application can make.

## Pillars

Every part of Consolpp is held to three requirements:

- **Performance**: fast, with the hot paths measured.
- **Memory**: safe and efficient, without undefined behavior, leaks or allocations that are not needed.
- **Thread safety**: correct in programs with several threads; each type states what it allows across threads.

## Platforms

- **Linux and macOS**: any distribution and version.

## Input

- **Argument parsing**: turns argv into the options and arguments a command declares.
- **Command definitions**: names, aliases, options and arguments, declared and validated at compile time.
- **Namespaces**: commands grouped by the part of their name before a colon, e.g. `cache:clear`.
- **Interactive input**: reading answers from the user, such as questions and confirmations.
- **Missing required arguments**: asked for interactively instead of failing.

## Execution

- **Command execution**: finds a command by its name or alias and runs it with its parsed input.
- **Single-command mode**: runs exactly one command without a command name in argv, e.g. `tool a.txt`.
- **Suggestions**: similar command names offered when a name matches no command.
- **Exit codes**: commands report their outcome to the calling process through the exit code.
- **Signals**: commands can react to signals such as an interrupt or a resized terminal.

## Output

- **Styled output**: colors, text attributes and links, adapted to what the terminal supports.
- **Markup tags**: every named tag is a style definition, with none built into the parser. `info`, `comment`, `question` and `error` are ready-made definitions the consumer can replace, and the consumer adds their own tags the same way.
- **Verbosity**: levels that decide which output is written.
- **Pipes**: input and output work the same when they are pipes or files instead of a terminal.

## Documentation

- **Help and list commands**: ready-made `help` and `list` commands, so that an application does not have to write its own.
- **Help formats**: help for a command and for the command list, as text, JSON and Markdown.
- **Man pages**: generated from the command definitions.
- **Shell completion**: Bash first, built on a shell-independent core so that other shells can follow.
- **Completion values**: suggestions for the values of options and arguments, not only their names.

## Terminal UI

- **Sections**: regions of the output that can be changed after they were written.
- **Text input**: line editing, hidden input for passwords and completion while typing.
- **Choice lists**: single and multiple selection with the arrow keys.
- **Input forms**: several inputs filled in together.

Further terminal UI features are added here by name once they are decided.

## Testing

- **Testing helpers**: running a command in-process with fake input and output.

## Deferred

Not now, only on demand. Nothing listed here is implemented; it moves back into scope once it is demanded.

- **Progress bar**: progress of work with a known length.
- **Spinner**: progress of work with an unknown length.
- **Table**: rows and columns, aligned by their visible width.
- **Tree**: hierarchical data.
- **Boxes and panels**: borders and titles around content.
- **Key bindings**: actions bound to keys and key combinations.
- **Pager**: scrolling through output longer than the screen.
- **Full-screen mode**: the alternate screen, cursor control and redrawing on resize.
- **Mouse input**: clicks, scrolling and movement.

## Out of scope

- **Windows**: neither the console nor terminals running on it.
- **Command abbreviations**: running a command by a prefix of its name is implicit and can be ambiguous.
- **Message blocks**: titles, notes, warnings and similar blocks are easy to build from panels.
- **Logging**: Consolpp is no logging library; its output integrates easily into whatever logging the consumer builds.
