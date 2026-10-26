#include "Completion/ValuesCommand.hpp"
#include "Consolpp/CommandManager.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Markup/AddTag.hpp"
#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Input/ArgumentsCommand.hpp"
#include "Input/FormCommand.hpp"
#include "Input/PromptsCommand.hpp"
#include "Output/PipeCommand.hpp"
#include "Output/SectionsCommand.hpp"
#include "Output/StylesCommand.hpp"
#include "Output/VerbosityCommand.hpp"
#include "Run/ExitCodeCommand.hpp"
#include "Run/SignalsCommand.hpp"

#include <exception>

#include <unistd.h>

int main([[maybe_unused]] const int argc, char** const argv)
{
    Consolpp::Event::Loop loop;
    Consolpp::Stream::OutputStream standardOutput{STDOUT_FILENO, Consolpp::Stream::FlushMode::Line};
    Consolpp::Stream::ErrorOutputStream standardErrorOutput{STDERR_FILENO, Consolpp::Stream::FlushMode::Immediate};
    Consolpp::Stream::InputStream standardInput{STDIN_FILENO, standardOutput, loop};
    Demo::Input::ArgumentsCommand argumentsCommand;
    Demo::Input::PromptsCommand promptsCommand;
    Demo::Input::FormCommand formCommand;
    Demo::Output::StylesCommand stylesCommand;
    Demo::Output::VerbosityCommand verbosityCommand;
    Demo::Output::PipeCommand pipeCommand;
    Demo::Output::SectionsCommand sectionsCommand;
    Demo::Run::SignalsCommand signalsCommand;
    Demo::Run::ExitCodeCommand exitCodeCommand;
    Demo::Completion::ValuesCommand valuesCommand;

    Consolpp::Markup::addTag(Consolpp::Markup::TagDefinition{
        .name = "highlight",
        .style =
            Consolpp::Markup::Style{
                .foreground = Consolpp::Markup::Color::black,
                .background = Consolpp::Markup::Color::brightCyan,
                .bold = true,
            },
    });

    const auto commandManager = Consolpp::CommandManager{standardInput, standardOutput, standardErrorOutput, loop}
                                    .addCommand(argumentsCommand)
                                    .addCommand(promptsCommand)
                                    .addCommand(formCommand)
                                    .addCommand(stylesCommand)
                                    .addCommand(verbosityCommand)
                                    .addCommand(pipeCommand)
                                    .addCommand(sectionsCommand)
                                    .addCommand(signalsCommand)
                                    .addCommand(exitCodeCommand)
                                    .addCommand(valuesCommand);

    try {
        return commandManager.run(argv);
    } catch (const std::exception& exception) {
        standardErrorOutput.write("Error: <error>{}</error>\n", exception.what());

        return 1;
    }
}
