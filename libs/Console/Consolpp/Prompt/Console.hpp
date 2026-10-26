#pragma once

#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/DefaultKeyBindings.hpp"
#include "Consolpp/Prompt/FormAnswer.hpp"
#include "Consolpp/Prompt/FormField.hpp"
#include "Consolpp/Prompt/KeyBinding.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Consolpp::Prompt
{
    // Asks fields on the terminal behind the input and output. While a field is asked, the terminal is in raw mode and
    // the other registrations of the loop keep running. Not thread-safe, like Loop.
    class Console
    {
        private:
            Event::Loop* loop;
            int inputDescriptor;
            Stream::OutputStream* output;
            std::span<const KeyBinding> keyBindings;
            std::string unconsumed;

        public:
            Console(Event::Loop& loop,
                    int inputDescriptor,
                    Stream::OutputStream& output,
                    std::span<const KeyBinding> keyBindings = defaultKeyBindings);

            // The field is copied, so it can be asked again. Throws CancelledError when the field is cancelled or the
            // input ends.
            // The typed-ahead bytes count as typed into the field.
            std::string ask(const TextField& field, std::string_view typedAhead = {});

            // The highlighted choice of a single selection, the selected ones otherwise. Throws CancelledError like the
            // text field.
            std::vector<std::size_t> ask(const ChoiceField& field, std::string_view typedAhead = {});

            // The typed answer or the default value. Throws CancelledError like the text field.
            bool ask(const ConfirmationField& field, std::string_view typedAhead = {});

            // The answers of the fields in their order. Enter submits a field and goes on to the next, the previous
            // field is asked again by the PreviousField action, and cancelling any field cancels the form. Throws
            // CancelledError like the text field.
            std::vector<FormAnswer> ask(std::span<const FormField> fields, std::string_view typedAhead = {});

            // The bytes read after the key that ended the last asked field, typed ahead for what follows.
            [[nodiscard]]
            std::string takeUnconsumed()
            {
                return std::exchange(this->unconsumed, std::string{});
            }
    };
} // namespace Consolpp::Prompt
