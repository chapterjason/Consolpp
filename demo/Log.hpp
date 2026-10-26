#pragma once

#include <cstddef>
#include <format>
#include <iostream>
#include <meta>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>

namespace Demo
{
    template <typename Value>
    inline constexpr bool isOptional = false;

    template <typename Value>
    inline constexpr bool isOptional<std::optional<Value>> = true;

    inline constexpr std::size_t indentWidth = 4;

    inline void writeLineStart(std::ostream& stream, const std::size_t depth)
    {
        stream << '\n' << std::string(depth * indentWidth, ' ');
    }

    template <typename Value>
    void write(std::ostream& stream, const Value& value, const std::size_t depth = 0)
    {
        constexpr std::meta::access_context everything = std::meta::access_context::unchecked();

        if constexpr (std::is_same_v<Value, bool>) {
            stream << (value ? "true" : "false");
        } else if constexpr (std::is_same_v<Value, signed char> || std::is_same_v<Value, unsigned char>) {
            stream << +value;
        } else if constexpr (std::is_enum_v<Value>) {
            template for (constexpr std::meta::info enumerator :
                          std::define_static_array(std::meta::enumerators_of(^^Value)))
            {
                if (value == [:enumerator:]) {
                    stream << std::meta::identifier_of(enumerator);

                    return;
                }
            }

            stream << +static_cast<std::underlying_type_t<Value>>(value);
        } else if constexpr (requires { stream << value; }) {
            stream << value;
        } else if constexpr (requires { std::formatter<Value, char>{}; }) {
            stream << std::format("{}", value);
        } else if constexpr (isOptional<Value>) {
            if (value.has_value()) {
                Demo::write(stream, *value, depth);
            } else {
                stream << "nullopt";
            }
        } else if constexpr (requires {
                                 value.begin();
                                 value.end();
                             }) {
            bool empty = true;

            stream << '[';

            for (const auto& element : value) {
                stream << (empty ? "" : ",");
                empty = false;
                Demo::writeLineStart(stream, depth + 1);
                Demo::write(stream, element, depth + 1);
            }

            if (!empty) {
                Demo::writeLineStart(stream, depth);
            }

            stream << ']';
        } else if constexpr (std::is_class_v<Value>) {
            bool empty = true;

            stream << std::meta::display_string_of(^^Value) << " {";

            template for (constexpr std::meta::info base :
                          std::define_static_array(std::meta::bases_of(^^Value, everything)))
            {
                stream << (empty ? "" : ",");
                empty = false;
                Demo::writeLineStart(stream, depth + 1);
                Demo::write(stream, value.[:base:], depth + 1);
            }

            template for (constexpr std::meta::info member :
                          std::define_static_array(std::meta::nonstatic_data_members_of(^^Value, everything)))
            {
                stream << (empty ? "" : ",");
                empty = false;
                Demo::writeLineStart(stream, depth + 1);

                if constexpr (std::meta::has_identifier(member)) {
                    stream << std::meta::identifier_of(member) << " = ";
                }

                Demo::write(stream, value.[:member:], depth + 1);
            }

            if (!empty) {
                Demo::writeLineStart(stream, depth);
            }

            stream << '}';
        } else {
            stream << '<' << std::meta::display_string_of(^^Value) << '>';
        }
    }

    // Writes the values to the standard output, each on its own lines, like console.log().
    template <typename... Values>
    void log(const Values&... values)
    {
        ((Demo::write(std::cout, values), std::cout << '\n'), ...);
    }
} // namespace Demo
