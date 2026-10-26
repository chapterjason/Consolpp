#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/GlobalOptions.hpp"
#include "Consolpp/Markup/ValidateSyntax.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace Consolpp
{
    template <std::size_t OptionCount, std::size_t ArgumentCount>
    class Signature
    {
        private:
            std::array<Option, OptionCount> options;
            std::array<Argument, ArgumentCount> arguments;

            static consteval void validateOptionValue(const Option& option)
            {
                if (option.repeatable && option.value == OptionValue::None) {
                    throw std::invalid_argument{"A repeatable option must take a value"};
                }

                if (option.defaultValue.has_value() && (option.value == OptionValue::None || option.repeatable)) {
                    throw std::invalid_argument{"A default value needs an option that takes a single value"};
                }

                if (!option.defaultValues.empty() && !option.repeatable) {
                    throw std::invalid_argument{"Default values need a repeatable option"};
                }

                if (option.defaultEnabled.has_value() && option.value != OptionValue::None) {
                    throw std::invalid_argument{"A default state needs a flag"};
                }

                if ((!option.suggestedValues.empty() || option.completion != ValueCompletion::None)
                    && option.value == OptionValue::None) {
                    throw std::invalid_argument{"A flag has no value to complete"};
                }

                if (!option.suggestedValues.empty() && option.completion != ValueCompletion::None) {
                    throw std::invalid_argument{"An option takes suggested values or a completion mode, not both"};
                }
            }

            consteval void validateOption(const Option& option) const
            {
                if (option.name.empty()) {
                    throw std::invalid_argument{"An option name must not be empty"};
                }

                if (option.name.starts_with('-')) {
                    throw std::invalid_argument{"An option name must not start with a dash"};
                }

                if (option.name.find('=') != std::string_view::npos) {
                    throw std::invalid_argument{"An option name must not contain an equals sign"};
                }

                if (option.shortcut == '-') {
                    throw std::invalid_argument{"An option shortcut must not be a dash"};
                }

                for (const Option& global : globalOptions) {
                    if (option.name == global.name
                        || (global.shortcut.has_value() && option.shortcut == global.shortcut)) {
                        throw std::invalid_argument{"A global option, its name or shortcut belongs to every command"};
                    }
                }

                Signature::validateOptionValue(option);
                Markup::validateSyntax(option.description);

                if (std::ranges::count_if(this->options,
                                          [&option](const Option& other) {
                                              return other.name == option.name;
                                          })
                    > 1) {
                    throw std::invalid_argument{"Two options must not share a name"};
                }

                if (option.shortcut.has_value() && std::ranges::count_if(this->options, [&option](const Option& other) {
                                                       return other.shortcut == option.shortcut;
                                                   }) > 1) {
                    throw std::invalid_argument{"Two options must not share a shortcut"};
                }

                if (option.value == OptionValue::None
                    && std::ranges::any_of(this->options, [&option](const Option& other) {
                           return other.name.starts_with("no-") && other.name.substr(3) == option.name;
                       })) {
                    throw std::invalid_argument{"A flag must not clash with an option named no-<name>"};
                }
            }

            consteval void validateArgument(const Argument& argument, const bool optionalBefore) const
            {
                if (argument.name.empty()) {
                    throw std::invalid_argument{"An argument name must not be empty"};
                }

                if (argument.variadic && &argument != &this->arguments.back()) {
                    throw std::invalid_argument{"Only the last argument can be variadic"};
                }

                if (argument.required && optionalBefore) {
                    throw std::invalid_argument{"A required argument must not follow an optional one"};
                }

                if (argument.defaultValue.has_value() && (argument.required || argument.variadic)) {
                    throw std::invalid_argument{"A default value needs an optional argument that is not variadic"};
                }

                if (!argument.defaultValues.empty() && (argument.required || !argument.variadic)) {
                    throw std::invalid_argument{"Default values need an optional variadic argument"};
                }

                if (!argument.suggestedValues.empty() && argument.completion != ValueCompletion::None) {
                    throw std::invalid_argument{"An argument takes suggested values or a completion mode, not both"};
                }

                Markup::validateSyntax(argument.description);

                if (std::ranges::count_if(this->arguments,
                                          [&argument](const Argument& other) {
                                              return other.name == argument.name;
                                          })
                    > 1) {
                    throw std::invalid_argument{"Two arguments must not share a name"};
                }
            }

        public:
            consteval Signature(const std::array<Option, OptionCount>& options,
                                const std::array<Argument, ArgumentCount>& arguments)
                : options{options},
                  arguments{arguments}
            {
                bool optionalBefore = false;

                for (const Option& option : this->options) {
                    this->validateOption(option);
                }

                for (const Argument& argument : this->arguments) {
                    this->validateArgument(argument, optionalBefore);

                    optionalBefore = optionalBefore || !argument.required;
                }
            }

            [[nodiscard]]
            constexpr const std::array<Option, OptionCount>& getOptions() const
            {
                return this->options;
            }

            [[nodiscard]]
            constexpr const std::array<Argument, ArgumentCount>& getArguments() const
            {
                return this->arguments;
            }
    };
} // namespace Consolpp
