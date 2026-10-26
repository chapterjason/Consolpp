#include "Consolpp/CommandManager.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Single/GreetCommand.hpp"

#include <exception>

#include <unistd.h>

int main([[maybe_unused]] const int argc, char** const argv)
{
    Consolpp::Event::Loop loop;
    Consolpp::Stream::OutputStream standardOutput{STDOUT_FILENO, Consolpp::Stream::FlushMode::Line};
    Consolpp::Stream::ErrorOutputStream standardErrorOutput{STDERR_FILENO, Consolpp::Stream::FlushMode::Immediate};
    Consolpp::Stream::InputStream standardInput{STDIN_FILENO, standardOutput, loop};
    Demo::Single::GreetCommand greetCommand;
    const auto commandManager = Consolpp::CommandManager{standardInput, standardOutput, standardErrorOutput, loop}
                                    .addCommand(greetCommand)
                                    .setDefaultCommand<Demo::Single::GreetCommand>(true);

    try {
        return commandManager.run(argv);
    } catch (const std::exception& exception) {
        standardErrorOutput.write("Error: <error>{}</error>\n", exception.what());

        return 1;
    }
}
