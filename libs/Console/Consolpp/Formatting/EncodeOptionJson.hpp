#pragma once

#include "Consolpp/Formatting/EncodeJsonArray.hpp"
#include "Consolpp/Formatting/EncodeJsonString.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"

#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // One field per member of Option; the description markup is stripped.
    template <std::output_iterator<char> Output>
    Output encodeOptionJson(const Option& option, Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto getValueName = [](const OptionValue value) -> std::string_view {
            switch (value) {
                case OptionValue::None:
                    return R"("none")";
                case OptionValue::Required:
                    return R"("required")";
                case OptionValue::Optional:
                    return R"("optional")";
            }

            return R"("none")";
        };
        std::optional<std::string_view> shortcut{};
        std::string description;

        if (option.shortcut.has_value()) {
            shortcut = std::string_view{&*option.shortcut, 1};
        }

        Styling::stripMarkup(option.description, std::back_inserter(description));

        copy(R"({"name":)");
        output = Formatting::encodeJsonString(option.name, output);
        copy(R"(,"shortcut":)");
        output = Formatting::encodeJsonString(shortcut, output);
        copy(R"(,"value":)");
        copy(getValueName(option.value));
        copy(R"(,"repeatable":)");
        copy(option.repeatable ? "true" : "false");
        copy(R"(,"description":)");
        output = Formatting::encodeJsonString(std::string_view{description}, output);
        copy(R"(,"defaultValue":)");
        output = Formatting::encodeJsonString(option.defaultValue, output);
        copy(R"(,"defaultValues":)");
        output = Formatting::encodeJsonArray(option.defaultValues, output);
        copy(R"(,"defaultEnabled":)");

        if (option.defaultEnabled.has_value()) {
            copy(*option.defaultEnabled ? "true" : "false");
        } else {
            copy("null");
        }

        copy("}");

        return output;
    }
} // namespace Consolpp::Formatting
