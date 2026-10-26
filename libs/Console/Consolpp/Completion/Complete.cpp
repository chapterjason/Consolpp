#include "Consolpp/Completion/Complete.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/ComputeValues.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Completion/Result.hpp"
#include "Consolpp/Completion/ValueTarget.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/GlobalOptions.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Consolpp::Completion
{
    namespace
    {
        void addOption(const std::string_view prefix,
                       const std::string_view name,
                       const std::string_view typed,
                       std::vector<std::string>& suggestions)
        {
            std::string suggestion{prefix};

            suggestion += name;

            if (suggestion.starts_with(typed)) {
                suggestions.push_back(std::move(suggestion));
            }
        }

        // Before the command name only global options are read, so they are the only options suggested there.
        Result completeBeforeCommand(const CommandList& commands, const std::string_view typed)
        {
            Result result;

            if (typed.starts_with('-')) {
                for (const Option& option : globalOptions) {
                    addOption("--", option.name, typed, result.suggestions);
                }

                return result;
            }

            for (const std::string_view name : commands.names) {
                if (name.starts_with(typed)) {
                    result.suggestions.emplace_back(name);
                }
            }

            return result;
        }

        Result completeOptionNames(const DefinitionView& definition, const std::string_view typed)
        {
            Result result;

            for (const Option& option : definition.options) {
                addOption("--", option.name, typed, result.suggestions);

                if (option.value == OptionValue::None) {
                    addOption("--no-", option.name, typed, result.suggestions);
                }
            }

            for (const Option& option : globalOptions) {
                addOption("--", option.name, typed, result.suggestions);
            }

            return result;
        }

        // The suggestions are whole words: the value prefix followed by the value.
        Result completeValue(const std::string_view command,
                             const std::span<const std::string_view> suggestedValues,
                             const ValueCompletion completion,
                             const Request& request,
                             const std::string_view valuePrefix,
                             const ComputeValues& computeValues)
        {
            Result result;
            const auto add = [&result, &request, valuePrefix](const std::string_view value) {
                if (!value.starts_with(request.value)) {
                    return;
                }

                std::string suggestion{valuePrefix};

                suggestion += value;
                result.suggestions.push_back(std::move(suggestion));
            };

            if (completion == ValueCompletion::File
                || completion == ValueCompletion::Directory
                || completion == ValueCompletion::Path) {
                result.paths = completion;
                result.valuePrefix = valuePrefix;

                return result;
            }

            if (completion == ValueCompletion::Computed) {
                for (const std::string& value : computeValues(command, request)) {
                    add(value);
                }

                return result;
            }

            for (const std::string_view value : suggestedValues) {
                add(value);
            }

            return result;
        }

        Result completeOptionValue(const DefinitionView& definition,
                                   const Option& option,
                                   const std::string_view value,
                                   const std::string_view valuePrefix,
                                   const ComputeValues& computeValues)
        {
            return completeValue(definition.name,
                                 option.suggestedValues,
                                 option.completion,
                                 Request{.target = ValueTarget::Option, .name = option.name, .value = value},
                                 valuePrefix,
                                 computeValues);
        }

        // Read like the parser reads it: the option whose value is the next word, or nullptr.
        const Option* findOptionBeforeValue(const std::span<const Option> options, const Parsing::Token& token)
        {
            if (token.getKind() == Parsing::TokenKind::LongOption) {
                const auto found = std::ranges::find(options, token.getName(), &Option::name);
                const bool valueNext = found != options.end()
                                       && found->value == OptionValue::Required
                                       && !token.getValue().has_value();

                return valueNext ? &*found : nullptr;
            }

            if (token.getKind() != Parsing::TokenKind::ShortOptions) {
                return nullptr;
            }

            const std::string_view shortcuts = token.getName();

            for (std::size_t position = 0; position < shortcuts.size(); ++position) {
                const auto found = std::ranges::find(options, shortcuts.at(position), &Option::shortcut);

                if (found != options.end() && found->value != OptionValue::None) {
                    const bool valueNext = found->value == OptionValue::Required && position + 1 == shortcuts.size();

                    return valueNext ? &*found : nullptr;
                }
            }

            return nullptr;
        }

        // "--name=value" completes the value, "--name" the option names.
        Result completeLongOption(const DefinitionView& definition,
                                  const std::string_view typed,
                                  const ComputeValues& computeValues)
        {
            const std::size_t equals = typed.find('=');

            if (equals == std::string_view::npos) {
                return completeOptionNames(definition, typed);
            }

            const auto found = std::ranges::find(definition.options, typed.substr(2, equals - 2), &Option::name);

            if (found == definition.options.end() || found->value == OptionValue::None) {
                return Result{};
            }

            return completeOptionValue(definition,
                                       *found,
                                       typed.substr(equals + 1),
                                       typed.substr(0, equals + 1),
                                       computeValues);
        }

        // In "-fvalue" the first shortcut that takes a value takes the rest of the word as its value.
        Result completeShortOptions(const DefinitionView& definition,
                                    const std::string_view typed,
                                    const ComputeValues& computeValues)
        {
            for (std::size_t position = 1; position < typed.size(); ++position) {
                const auto found = std::ranges::find(definition.options, typed.at(position), &Option::shortcut);

                if (found == definition.options.end() || position + 1 == typed.size()) {
                    break;
                }

                if (found->value != OptionValue::None) {
                    return completeOptionValue(definition,
                                               *found,
                                               typed.substr(position + 1),
                                               typed.substr(0, position + 1),
                                               computeValues);
                }
            }

            return completeOptionNames(definition, typed);
        }

        // The words given to the command before the typed one.
        Result completeCommand(const DefinitionView& definition,
                               const std::span<const char* const> given,
                               const std::string_view typed,
                               const ComputeValues& computeValues)
        {
            const Option* valueOf = nullptr;
            bool afterSeparator = false;
            std::size_t position = 0;

            for (auto word = given.begin(); word != given.end(); ++word) {
                const Parsing::Token token = *Parsing::TokenIterator{word};
                const Parsing::TokenKind kind = token.getKind();

                if (valueOf != nullptr) {
                    valueOf = nullptr;
                } else if (afterSeparator
                           || kind == Parsing::TokenKind::Positional
                           || kind == Parsing::TokenKind::Dash) {
                    ++position;
                } else if (kind == Parsing::TokenKind::Separator) {
                    afterSeparator = true;
                } else {
                    valueOf = findOptionBeforeValue(definition.options, token);
                }
            }

            if (valueOf != nullptr) {
                return completeOptionValue(definition, *valueOf, typed, std::string_view{}, computeValues);
            }

            if (!afterSeparator && typed.starts_with("--")) {
                return completeLongOption(definition, typed, computeValues);
            }

            if (!afterSeparator && typed.starts_with('-')) {
                return completeShortOptions(definition, typed, computeValues);
            }

            const std::span<const Argument> arguments = definition.arguments;
            const bool beyondLast = position >= arguments.size();

            if (arguments.empty() || (beyondLast && !arguments.back().variadic)) {
                return Result{};
            }

            const Argument& argument = beyondLast
                                           ? arguments.back()
                                           : *std::next(arguments.begin(), static_cast<std::ptrdiff_t>(position));

            return completeValue(definition.name,
                                 argument.suggestedValues,
                                 argument.completion,
                                 Request{.target = ValueTarget::Argument, .name = argument.name, .value = typed},
                                 std::string_view{},
                                 computeValues);
        }

        std::span<const DefinitionView>::iterator findDefinition(const std::span<const DefinitionView> definitions,
                                                                 const std::string_view name)
        {
            return std::ranges::find_if(definitions, [name](const DefinitionView& definition) {
                return definition.name == name
                       || std::ranges::find(definition.aliases, name) != definition.aliases.end();
            });
        }
    } // namespace

    Result complete(const CommandList& commands,
                    const std::string_view singleCommand,
                    const std::span<const char* const> words,
                    const std::size_t current,
                    const ComputeValues& computeValues)
    {
        if (current >= words.size()) {
            throw std::out_of_range{"The current word is not one of the words"};
        }

        const std::string_view typed{*std::next(words.begin(), static_cast<std::ptrdiff_t>(current))};
        const std::span<const char* const> before = words.first(current);

        if (!singleCommand.empty()) {
            const auto definition = findDefinition(commands.definitions, singleCommand);

            if (definition == commands.definitions.end()) {
                return Result{};
            }

            return completeCommand(*definition, before, typed, computeValues);
        }

        // Read like CommandManager reads argv: the command name is the first word that is no option.
        const auto commandName = std::ranges::find_if(before, [](const char* const word) {
            const std::string_view text{word};

            return !text.starts_with('-') || text == "--";
        });

        if (commandName == before.end()) {
            return completeBeforeCommand(commands, typed);
        }

        const auto definition = findDefinition(commands.definitions, *commandName);

        if (definition == commands.definitions.end()) {
            return Result{};
        }

        return completeCommand(*definition,
                               std::span<const char* const>{std::next(commandName), before.end()},
                               typed,
                               computeValues);
    }
} // namespace Consolpp::Completion
