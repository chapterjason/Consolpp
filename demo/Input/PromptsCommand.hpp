#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Prompt/Autocompletion.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/Echo.hpp"
#include "Consolpp/Prompt/Selection.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace Demo::Input
{
    class PromptsCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "input:prompts",
                .description = "Asks each kind of field on its own",
                .help = "Tab takes the suggestion of the project name and Up and Down change it. Space toggles a "
                        "feature, Enter submits a field, and Escape or Ctrl+C cancel it.",
                .signature = Consolpp::Signature{std::array<Consolpp::Option, 0>{},
                                                 std::array<Consolpp::Argument, 0>{}},
            };

            static int run(Consolpp::Stream::InputStream& input, Consolpp::Stream::OutputStream& output)
            {
                static constexpr std::array<std::string_view, 3> names{"consolpp", "console-tools", "conductor"};
                static constexpr std::array<std::string_view, 3> environments{"development", "staging", "production"};
                static constexpr std::array<std::size_t, 1> staging{1};
                static constexpr std::array<std::string_view, 4> features{"Logging", "Metrics", "Tracing", "Caching"};
                static constexpr std::array<std::size_t, 2> recommended{0, 1};

                const std::string name = input.ask(Consolpp::Prompt::TextField{Consolpp::Prompt::TextFieldOptions{
                    .label = "Project name",
                    .defaultValue = "consolpp",
                    .autocompletion = Consolpp::Prompt::Autocompletion::fromValues(names),
                }});
                const std::string password = input.ask(Consolpp::Prompt::TextField{
                    Consolpp::Prompt::TextFieldOptions{.label = "Password", .echo = Consolpp::Prompt::Echo::Masked}});
                const std::string token = input.ask(Consolpp::Prompt::TextField{
                    Consolpp::Prompt::TextFieldOptions{.label = "Token", .echo = Consolpp::Prompt::Echo::Hidden}});
                const std::vector<std::size_t> environment = input.ask(
                    Consolpp::Prompt::ChoiceField{Consolpp::Prompt::ChoiceFieldOptions{
                        .label = "Environment",
                        .choices = environments,
                        .defaultIndexes = staging,
                    }});
                const std::vector<std::size_t> enabled = input.ask(
                    Consolpp::Prompt::ChoiceField{Consolpp::Prompt::ChoiceFieldOptions{
                        .label = "Features",
                        .choices = features,
                        .selection = Consolpp::Prompt::Selection::Multiple,
                        .defaultIndexes = recommended,
                    }});
                const bool confirmed = input.ask(Consolpp::Prompt::ConfirmationField{
                    Consolpp::Prompt::ConfirmationFieldOptions{.label = "Create the project?", .defaultValue = true}});

                output.write("\n<info>Project</info> {} for <comment>{}</comment> with {} of {} features, a password "
                             "of {} and a token of {} characters: {}.\n",
                             name,
                             environments.at(environment.front()),
                             enabled.size(),
                             features.size(),
                             password.size(),
                             token.size(),
                             confirmed ? "created" : "not created");

                return 0;
            }
    };
} // namespace Demo::Input
