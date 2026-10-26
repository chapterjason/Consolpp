# Output

`Consolpp::Stream::OutputStream` for standard output, `Consolpp::Stream::ErrorOutputStream` for standard error. A command takes them as `run` parameters.

## Writing

```cpp
output.write("Uploaded <info>{}</info> files\n", count);
output << "<info>Done</info> " << 42 << '\n';
output.writeRaw("<not markup>\n");
```

- `write` formats like `std::format`. The format is [markup](MARKUP.md); the arguments are inserted as plain text, so values never inject markup.
- Invalid markup throws before anything is written.
- `<<` reads strings as markup too; each string must be complete markup. Numbers, `bool` and `char` are written as they are.
- `writeRaw` writes the text exactly as it is.
- `disableMarkup()` / `enableMarkup()` switch markup reading off and on.
- Styles are left out when the output is no terminal or colors are disabled by `NO_COLOR`; `FORCE_COLOR` forces them. `isDecorated()` tells.

## Verbosity

```cpp
output.write(Consolpp::Stream::Verbosity::Verbose, "Reading <comment>{}</comment>\n", path);
```

Levels: `Silent`, `Quiet`, `Normal`, `Verbose`, `VeryVerbose`, `Debug`. A message is written when its level is at most the stream's verbosity. `write` without a level is `Normal`. The global options `-q`, `--silent`, `-v`, `-vv`, `-vvv` set the verbosity of both streams.

Write errors at `Quiet` to the error output, so `-q` keeps them:

```cpp
errorOutput.write(Consolpp::Stream::Verbosity::Quiet, "<error>{}</error>\n", message);
```

Checks: `isQuiet()`, `isVerbose()`, `isVeryVerbose()`, `isDebug()`, `isShown(level)`.

## Flushing

`FlushMode::Buffered`, `Line` or `Immediate`, set in the constructor or with `setFlushMode()`. `flush()` writes what is buffered.

Flush before waiting, e.g. before `loop.run()`, so the user sees the output.

To write many lines in one piece:

```cpp
const Consolpp::Stream::BufferingScope buffering{output};
```

buffers until the scope ends and then restores the previous mode.

## Terminal

- `isTerminal()`: whether the output is a terminal.
- `getColumns()`, `getRows()`: the size of the terminal.
- `getAvailableWidth()`: the width to write at, one column less than the terminal has.

## Into a string

```cpp
std::string text;
Consolpp::Stream::OutputStream output{text};
```

The string must outlive the stream. Undecorated unless `OutputStream{text, true}`.
