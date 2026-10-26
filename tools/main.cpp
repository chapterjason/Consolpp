#include "Consolpp/CommandManager.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Screen/RecordCommand.hpp"
#include "Screen/ReplayCommand.hpp"

#include <exception>

#include <unistd.h>

namespace
{
    int run(const char* const* const argv)
    {
        Consolpp::Stream::ErrorOutputStream errorOutput{STDERR_FILENO, Consolpp::Stream::FlushMode::Immediate};

        try {
            Consolpp::Event::Loop loop;
            Consolpp::Stream::OutputStream output{STDOUT_FILENO, Consolpp::Stream::FlushMode::Buffered};
            Consolpp::Stream::InputStream input{STDIN_FILENO, output, loop};
            Consolpp::Tools::Screen::RecordCommand record;
            Consolpp::Tools::Screen::ReplayCommand replay;
            const auto commands = Consolpp::CommandManager{input, output, errorOutput, loop}
                                      .addCommand(record)
                                      .addCommand(replay);

            return commands.run(argv);
        } catch (const std::exception& exception) {
            errorOutput.write("Error: <error>{}</error>\n", exception.what());

            return 1;
        }
    }
} // namespace

// main has to take a non-const argv.
int main([[maybe_unused]] const int argc, char** const argv) // NOLINT(misc-const-correctness)
{
    try {
        return run(argv);
    } catch (...) {
        // Only reached when even the error message cannot be written.
        return 1;
    }
}
