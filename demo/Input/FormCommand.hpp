#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/Form.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <cstddef>
#include <string_view>

namespace Demo::Input
{
    class FormCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "input:form",
                .description = "Asks several fields together in a form",
                .help = "Enter submits a field and goes on to the next one, Shift+Tab goes back to the previous one, "
                        "and Escape or Ctrl+C cancel the form.",
                .signature = Consolpp::Signature{std::array<Consolpp::Option, 0>{},
                                                 std::array<Consolpp::Argument, 0>{}},
            };

            static int run(Consolpp::Stream::InputStream& input, Consolpp::Stream::OutputStream& output)
            {
                static constexpr std::array<std::string_view, 3> regions{"Europe", "America", "Asia"};
                static constexpr std::array<std::size_t, 1> europe{0};

                const auto [name, region, backups] = input.ask(Consolpp::Prompt::Form{
                    Consolpp::Prompt::TextField{
                        Consolpp::Prompt::TextFieldOptions{.label = "Server name", .defaultValue = "web-1"}},
                    Consolpp::Prompt::ChoiceField{Consolpp::Prompt::ChoiceFieldOptions{
                        .label = "Region",
                        .choices = regions,
                        .defaultIndexes = europe,
                    }},
                    Consolpp::Prompt::ConfirmationField{
                        Consolpp::Prompt::ConfirmationFieldOptions{.label = "Daily backups?", .defaultValue = false}},
                });

                output.write("\n<info>Server</info> {} in <comment>{}</comment> {} daily backups.\n",
                             name,
                             regions.at(region.front()),
                             backups ? "with" : "without");

                return 0;
            }
    };
} // namespace Demo::Input
