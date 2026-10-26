#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/BuiltIn/CompletionCommand.hpp"
#include "Consolpp/BuiltIn/HelpCommand.hpp"
#include "Consolpp/BuiltIn/ListCommand.hpp"
#include "Consolpp/BuiltIn/ManCommand.hpp"
#include "Consolpp/Command.hpp"
#include "Consolpp/CommandError.hpp"
#include "Consolpp/CommandErrorKind.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/Complete.hpp"
#include "Consolpp/Completion/FormatResult.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Completion/Result.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/GetCommandNames.hpp"
#include "Consolpp/GetCompleteArguments.hpp"
#include "Consolpp/GetRunArguments.hpp"
#include "Consolpp/HasUniqueCommandNames.hpp"
#include "Consolpp/IsSuggestion.hpp"
#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/ExtractGlobalOptions.hpp"
#include "Consolpp/Parsing/GlobalOptionValues.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/Parser.hpp"
#include "Consolpp/Parsing/RequestsHelp.hpp"
#include "Consolpp/Parsing/RequestsVersion.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/RunArgument.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Text/CompareNaturalIgnoringCase.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <compare>
#include <cstddef>
#include <iterator>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace Consolpp
{
    // list and help are built in; a command of the same name does not compile.
    template <Command... Commands>
        requires(hasUniqueCommandNames<BuiltIn::ListCommand,
                                       BuiltIn::HelpCommand,
                                       BuiltIn::ManCommand,
                                       BuiltIn::CompletionCommand,
                                       Commands...>())
    class CommandManager
    {
        private:
            template <Command... Others>
                requires(hasUniqueCommandNames<BuiltIn::ListCommand,
                                               BuiltIn::HelpCommand,
                                               BuiltIn::ManCommand,
                                               BuiltIn::CompletionCommand,
                                               Others...>())
            friend class CommandManager;

            std::tuple<Commands&...> commands;
            Stream::InputStream* input;
            Stream::OutputStream* output;
            Stream::ErrorOutputStream* errorOutput;
            Event::Loop* loop;
            // The name of the command run without a command name in argv; empty without one.
            std::string defaultCommand;
            bool singleCommand{false};
            // Without a name, run(argv) takes the file name of argv[0].
            std::string name;
            std::string version;

            static constexpr auto candidates = [] {
                auto names = Consolpp::getCommandNames<false,
                                                       BuiltIn::ListCommand,
                                                       BuiltIn::HelpCommand,
                                                       BuiltIn::ManCommand,
                                                       BuiltIn::CompletionCommand,
                                                       Commands...>();

                for (std::size_t index = 1; index < names.size(); ++index) {
                    const std::string_view name = names.at(index);
                    std::size_t target = index;

                    while (target > 0 && std::is_lt(Text::compareNaturalIgnoringCase(name, names.at(target - 1)))) {
                        names.at(target) = names.at(target - 1);
                        --target;
                    }

                    names.at(target) = name;
                }

                return names;
            }();

            static constexpr std::size_t maxCandidateLength = [] {
                std::size_t length = 0;

                for (const std::string_view name : CommandManager::candidates) {
                    length = std::max(length, name.size());
                }

                return length;
            }();

            static constexpr std::array<DefinitionView, sizeof...(Commands) + 4> definitions{
                DefinitionView::fromDefinition(BuiltIn::ListCommand::definition),
                DefinitionView::fromDefinition(BuiltIn::HelpCommand::definition),
                DefinitionView::fromDefinition(BuiltIn::ManCommand::definition),
                DefinitionView::fromDefinition(BuiltIn::CompletionCommand::definition),
                DefinitionView::fromDefinition(Commands::definition)...,
            };

            static constexpr CommandList commandList{
                .definitions = CommandManager::definitions,
                .names = CommandManager::candidates,
                .isSuggestion = &Consolpp::isSuggestion<CommandManager::maxCandidateLength>,
            };

            // Read with the options of the named command, so its option values do not count as a request.
            template <bool (*Requests)(std::span<const Option>, Parsing::TokenRange)>
            static bool isRequested(const std::string_view name, const std::span<const char* const> arguments)
            {
                const auto isRequest = [name, arguments]<Command Type> {
                    return CommandManager::matches<Type>(name)
                           && Requests(Type::definition.signature.getOptions(), Parsing::TokenRange{arguments});
                };

                return (isRequest.template operator()<Commands>() || ...)
                       || isRequest.template operator()<BuiltIn::ListCommand>()
                       || isRequest.template operator()<BuiltIn::HelpCommand>()
                       || isRequest.template operator()<BuiltIn::ManCommand>()
                       || isRequest.template operator()<BuiltIn::CompletionCommand>();
            }

            template <Command Type>
            static bool matches(const std::string_view name)
            {
                return Type::definition.name == name
                       || std::ranges::find(Type::definition.aliases, name) != Type::definition.aliases.end();
            }

            template <RunArgument Argument, const auto& CommandDefinition>
            [[nodiscard]]
            auto& getRunArgument(Parsing::Parameters<CommandDefinition>& parameters, const Program& program) const
            {
                if constexpr (Argument == RunArgument::Parameters) {
                    return parameters;
                } else if constexpr (Argument == RunArgument::Input) {
                    return *this->input;
                } else if constexpr (Argument == RunArgument::Output) {
                    return *this->output;
                } else if constexpr (Argument == RunArgument::ErrorOutput) {
                    return *this->errorOutput;
                } else if constexpr (Argument == RunArgument::Loop) {
                    return *this->loop;
                } else if constexpr (Argument == RunArgument::Program) {
                    return program;
                } else {
                    return CommandManager::commandList;
                }
            }

            template <const auto& CommandDefinition>
            void askMissingArguments(Parsing::Parameters<CommandDefinition>& parameters) const
            {
                std::size_t index = 0;

                for (const Argument& argument : CommandDefinition.signature.getArguments()) {
                    if (parameters.isArgumentMissing(index)) {
                        this->askArgument(argument, index, parameters);
                    }

                    ++index;
                }
            }

            // An empty answer is asked again; for the variadic argument it ends the values once there is one.
            template <const auto& CommandDefinition>
            void askArgument(const Argument& argument,
                             const std::size_t index,
                             Parsing::Parameters<CommandDefinition>& parameters) const
            {
                std::string label;

                Markup::encode(argument.name, std::back_inserter(label));

                const Prompt::TextField field{Prompt::TextFieldOptions{.label = label}};

                if (!argument.variadic) {
                    std::string answer;

                    while (answer.empty()) {
                        answer = this->input->ask(field);
                    }

                    parameters.setArgument(index, answer);

                    return;
                }

                std::vector<std::string> answers;

                while (true) {
                    std::string answer = this->input->ask(field);

                    if (answer.empty() && !answers.empty()) {
                        break;
                    }

                    if (!answer.empty()) {
                        answers.push_back(std::move(answer));
                    }
                }

                const std::vector<std::string_view> values{answers.begin(), answers.end()};

                parameters.setArgument(index, std::span<const std::string_view>{values});
            }

            void applyGlobalOptions(const Parsing::GlobalOptionValues& values) const
            {
                if (values.verbosity.has_value()) {
                    this->output->setVerbosity(*values.verbosity);
                    this->errorOutput->setVerbosity(*values.verbosity);
                }

                if (values.interactionDisabled) {
                    this->input->disableInteraction();
                }
            }

            template <Command Type>
            [[nodiscard]]
            int runCommand(Type& command, const std::span<const char* const> arguments, const Program& program) const
            {
                static constexpr auto runArguments = Consolpp::getRunArguments<Type>(&Type::run);
                std::vector<const char*> remaining;

                this->applyGlobalOptions(
                    Parsing::extractGlobalOptions(Type::definition.signature.getOptions(), arguments, remaining));

                Parsing::Parser<Type::definition> parser{Parsing::TokenRange{remaining}};
                Parsing::Parameters<Type::definition> parameters = parser.parse();

                if (this->input->isInteractive()) {
                    if constexpr (requires { &Type::interact; }) {
                        static constexpr auto interactArguments = Consolpp::getInteractArguments<Type>(&Type::interact);

                        [this, &command, &parameters, &program]<std::size_t... Indexes>(
                            std::index_sequence<Indexes...> /*indexes*/) {
                            command.interact(
                                this->getRunArgument<interactArguments.at(Indexes)>(parameters, program)...);
                        }(std::make_index_sequence<interactArguments.size()>{});
                    }

                    this->askMissingArguments(parameters);
                }

                parameters.requireArguments();

                return [this, &command, &parameters, &program]<std::size_t... Indexes>(
                           std::index_sequence<Indexes...> /*indexes*/) {
                    return command.run(this->getRunArgument<runArguments.at(Indexes)>(parameters, program)...);
                }(std::make_index_sequence<runArguments.size()>{});
            }

            template <std::size_t... Indexes>
            [[nodiscard]]
            int runMatching(const std::string_view name,
                            const std::span<const char* const> arguments,
                            const Program& program,
                            std::index_sequence<Indexes...> /*indexes*/) const
            {
                int exitCode = 0;
                const auto runIfMatching = [this, name, arguments, &program, &exitCode]<std::size_t Index> {
                    if (!CommandManager::matches<std::tuple_element_t<Index, std::tuple<Commands...>>>(name)) {
                        return false;
                    }

                    exitCode = this->runCommand(std::get<Index>(this->commands), arguments, program);

                    return true;
                };
                const auto runBuiltInIfMatching = [this, name, arguments, &program, &exitCode]<Command Type> {
                    if (!CommandManager::matches<Type>(name)) {
                        return false;
                    }

                    Type command;

                    exitCode = this->runCommand(command, arguments, program);

                    return true;
                };
                const bool found = (runIfMatching.template operator()<Indexes>() || ...)
                                   || runBuiltInIfMatching.template operator()<BuiltIn::ListCommand>()
                                   || runBuiltInIfMatching.template operator()<BuiltIn::HelpCommand>()
                                   || runBuiltInIfMatching.template operator()<BuiltIn::ManCommand>()
                                   || runBuiltInIfMatching.template operator()<BuiltIn::CompletionCommand>();

                if (!found) {
                    throw CommandError{
                        CommandErrorKind::UnknownCommand,
                        name,
                        CommandManager::candidates,
                        &Consolpp::isSuggestion<CommandManager::maxCandidateLength>,
                    };
                }

                return exitCode;
            }

            CommandManager(Stream::InputStream& input,
                           Stream::OutputStream& output,
                           Stream::ErrorOutputStream& errorOutput,
                           Event::Loop& loop,
                           const std::tuple<Commands&...> commands,
                           std::string defaultCommand,
                           const bool singleCommand,
                           std::string name,
                           std::string version)
                : commands{commands},
                  input{&input},
                  output{&output},
                  errorOutput{&errorOutput},
                  loop{&loop},
                  defaultCommand{std::move(defaultCommand)},
                  singleCommand{singleCommand},
                  name{std::move(name)},
                  version{std::move(version)}
            {
            }

            [[nodiscard]]
            int runHelp(const char* const name, const Program& program) const
            {
                const std::array<const char*, 1> names{name};
                BuiltIn::HelpCommand help;

                return this->runCommand(help, names, program);
            }

            [[nodiscard]]
            int runVersion(const Program& program) const
            {
                const std::string_view separator = program.name.empty() || program.version.empty() ? "" : " ";

                this->output->write("{}{}{}\n", program.name, separator, program.version);

                return 0;
            }

            template <Command Type>
            [[nodiscard]]
            int runBesideSingleCommand(const std::span<const char* const> given, const Program& program) const
            {
                const std::span<const char* const> rest = given.subspan(1);

                if (CommandManager::isRequested<&Parsing::requestsHelp>(Type::definition.name, rest)) {
                    return this->runHelp(given.front(), program);
                }

                if (CommandManager::isRequested<&Parsing::requestsVersion>(Type::definition.name, rest)) {
                    return this->runVersion(program);
                }

                Type command;

                return this->runCommand(command, rest, program);
            }

            template <Command Type>
            [[nodiscard]]
            static std::vector<std::string> computeValues(Type& command,
                                                          const Completion::Request& request,
                                                          const Program& program)
            {
                static constexpr auto completeArguments = Consolpp::getCompleteArguments(&Type::complete);
                const auto getCompleteArgument = [&program]<RunArgument Argument>() -> const auto& {
                    if constexpr (Argument == RunArgument::Program) {
                        return program;
                    } else {
                        return CommandManager::commandList;
                    }
                };

                return [&command, &request, &getCompleteArgument]<std::size_t... Indexes>(
                           std::index_sequence<Indexes...> /*indexes*/) {
                    return command.complete(
                        request,
                        getCompleteArgument.template operator()<completeArguments.at(Indexes)>()...);
                }(std::make_index_sequence<completeArguments.size()>{});
            }

            // The values the complete member of the named command computes.
            template <std::size_t... Indexes>
            [[nodiscard]]
            std::vector<std::string> computeMatchingValues(const std::string_view name,
                                                           const Completion::Request& request,
                                                           const Program& program,
                                                           std::index_sequence<Indexes...> /*indexes*/) const
            {
                std::vector<std::string> values;
                const auto computeIfMatching = [this, name, &request, &program, &values]<std::size_t Index> {
                    using Type = std::tuple_element_t<Index, std::tuple<Commands...>>;

                    if constexpr (requires { &Type::complete; }) {
                        if (CommandManager::matches<Type>(name)) {
                            values = CommandManager::computeValues(std::get<Index>(this->commands), request, program);
                        }
                    }
                };
                const auto computeBuiltInIfMatching = [name, &request, &program, &values]<Command Type> {
                    if constexpr (requires { &Type::complete; }) {
                        if (CommandManager::matches<Type>(name)) {
                            Type command;

                            values = CommandManager::computeValues(command, request, program);
                        }
                    }
                };

                (computeIfMatching.template operator()<Indexes>(), ...);
                computeBuiltInIfMatching.template operator()<BuiltIn::ListCommand>();
                computeBuiltInIfMatching.template operator()<BuiltIn::HelpCommand>();
                computeBuiltInIfMatching.template operator()<BuiltIn::ManCommand>();
                computeBuiltInIfMatching.template operator()<BuiltIn::CompletionCommand>();

                return values;
            }

            // The arguments of "_complete <current> -- <words>", which the completion script sends with the words after
            // the program name. Throws std::invalid_argument for other arguments.
            [[nodiscard]]
            int runCompletion(const std::span<const char* const> arguments, const Program& program) const
            {
                if (arguments.size() < 2 || std::string_view{*std::next(arguments.begin(), 1)} != "--") {
                    throw std::invalid_argument{"A completion request needs the current word and -- before the words"};
                }

                const std::string_view index{arguments.front()};
                const char* const first = std::to_address(index.begin());
                const char* const end = std::to_address(index.end());
                std::size_t current = 0;
                const std::from_chars_result parsed = std::from_chars(first, end, current);

                if (parsed.ec != std::errc{} || parsed.ptr != end) {
                    throw std::invalid_argument{"The current word of a completion request must be a number"};
                }

                const Completion::Result result = Completion::complete(
                    CommandManager::commandList,
                    this->singleCommand ? std::string_view{this->defaultCommand} : std::string_view{},
                    arguments.subspan(2),
                    current,
                    [this, &program](const std::string_view name, const Completion::Request& request) {
                        return this->computeMatchingValues(name,
                                                           request,
                                                           program,
                                                           std::index_sequence_for<Commands...>{});
                    });
                std::string answer;

                Completion::formatResult(result, std::back_inserter(answer));
                this->output->writeRaw(answer);

                return 0;
            }

            [[nodiscard]]
            int runWith(const Parsing::TokenRange tokens, const Program& program) const
            {
                const std::span<const char* const> given = tokens.getArguments();

                // Ahead of everything, as the words to complete belong to no command, also not to the single one.
                if (!given.empty() && std::string_view{given.front()} == "_complete") {
                    return this->runCompletion(given.subspan(1), program);
                }

                // The single command takes all of argv otherwise; after "--" the word reaches it as an argument.
                if (this->singleCommand && !given.empty() && std::string_view{given.front()} == "completion") {
                    return this->runBesideSingleCommand<BuiltIn::CompletionCommand>(given, program);
                }

                if (this->singleCommand && !given.empty() && std::string_view{given.front()} == "man") {
                    return this->runBesideSingleCommand<BuiltIn::ManCommand>(given, program);
                }

                // Before the command name only global options can stand; the command's own are read with it.
                const auto commandName = this->singleCommand
                                             ? given.begin()
                                             : std::ranges::find_if(given, [](const char* const argument) {
                                                   const std::string_view text{argument};

                                                   return !text.starts_with('-') || text == "--";
                                               });
                std::vector<const char*> leading;

                this->applyGlobalOptions(
                    Parsing::extractGlobalOptions(std::span<const Option>{},
                                                  std::span<const char* const>{given.begin(), commandName},
                                                  leading));
                leading.insert(leading.end(), commandName, given.end());

                const std::span<const char* const> arguments{leading};
                const bool hasDefaultCommand = !this->defaultCommand.empty();
                const bool helpWithoutCommand = !arguments.empty()
                                                && (std::string_view{arguments.front()} == "--help"
                                                    || std::string_view{arguments.front()} == "-h");
                const bool versionWithoutCommand = !arguments.empty()
                                                   && std::string_view{arguments.front()} == "--version";

                if (this->singleCommand
                    && CommandManager::isRequested<&Parsing::requestsHelp>(this->defaultCommand, arguments)) {
                    return this->runHelp(this->defaultCommand.c_str(), program);
                }

                if (this->singleCommand
                    && CommandManager::isRequested<&Parsing::requestsVersion>(this->defaultCommand, arguments)) {
                    return this->runVersion(program);
                }

                if (this->singleCommand || (hasDefaultCommand && arguments.empty())) {
                    return this->runMatching(this->defaultCommand,
                                             arguments,
                                             program,
                                             std::index_sequence_for<Commands...>{});
                }

                if (hasDefaultCommand && helpWithoutCommand) {
                    return this->runHelp(this->defaultCommand.c_str(), program);
                }

                if (versionWithoutCommand) {
                    return this->runVersion(program);
                }

                if (arguments.empty() || helpWithoutCommand) {
                    BuiltIn::ListCommand list;

                    return this->runCommand(list, std::span<const char* const>{}, program);
                }

                if (CommandManager::isRequested<&Parsing::requestsHelp>(arguments.front(), arguments.subspan(1))) {
                    return this->runHelp(arguments.front(), program);
                }

                if (CommandManager::isRequested<&Parsing::requestsVersion>(arguments.front(), arguments.subspan(1))) {
                    return this->runVersion(program);
                }

                return this->runMatching(arguments.front(),
                                         arguments.subspan(1),
                                         program,
                                         std::index_sequence_for<Commands...>{});
            }

        public:
            CommandManager(Stream::InputStream& input,
                           Stream::OutputStream& output,
                           Stream::ErrorOutputStream& errorOutput,
                           Event::Loop& loop)
                requires(sizeof...(Commands) == 0)
                : input{&input},
                  output{&output},
                  errorOutput{&errorOutput},
                  loop{&loop}
            {
            }

            template <Command Type>
            [[nodiscard]]
            CommandManager<Commands..., Type> addCommand(Type& command) const
            {
                return CommandManager<Commands..., Type>{
                    *this->input,
                    *this->output,
                    *this->errorOutput,
                    *this->loop,
                    std::tuple_cat(this->commands, std::tie(command)),
                    this->defaultCommand,
                    this->singleCommand,
                    this->name,
                    this->version,
                };
            }

            // The command runs when argv holds no command name. As the single command, argv never holds one: all of it
            // belongs to this command, and no other command can be reached, except completion as the first word.
            template <Command Type>
                requires(std::disjunction_v<std::is_same<Type, Commands>...>)
            [[nodiscard]]
            CommandManager setDefaultCommand(const bool isSingleCommand = false) const
            {
                CommandManager manager = *this;

                manager.defaultCommand = Type::definition.name;
                manager.singleCommand = isSingleCommand;

                return manager;
            }

            [[nodiscard]]
            CommandManager setName(const std::string_view programName) const
            {
                CommandManager manager = *this;

                manager.name = programName;

                return manager;
            }

            [[nodiscard]]
            CommandManager setVersion(const std::string_view programVersion) const
            {
                CommandManager manager = *this;

                manager.version = programVersion;

                return manager;
            }

            // argv as main receives it; argv[0] names the program unless a name is set.
            [[nodiscard]]
            int run(const char* const* const argv) const
                requires(sizeof...(Commands) > 0)
            {
                const std::string_view invoked = *argv == nullptr ? std::string_view{} : std::string_view{*argv};
                const std::string_view fileName = invoked.substr(invoked.rfind('/') + 1);

                return this->runWith(
                    Parsing::TokenRange::fromArgv(argv),
                    Program{.name = this->name.empty() ? fileName : this->name, .version = this->version});
            }

            [[nodiscard]]
            int run(const Parsing::TokenRange tokens) const
                requires(sizeof...(Commands) > 0)
            {
                return this->runWith(tokens, Program{.name = this->name, .version = this->version});
            }
    };
} // namespace Consolpp
