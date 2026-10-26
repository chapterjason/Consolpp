#pragma once

#include "Consolpp/Markup/EncodingIterator.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <string_view>

namespace Consolpp::Stream
{
    // An argument of a markup format, inserted as plain text and never read as markup.
    template <typename Value>
    struct PlainArgument
    {
        public:
            const Value* value;
    };
} // namespace Consolpp::Stream

// Specializing std::formatter for a program-defined type is allowed ([namespace.std], [format.formatter.spec]).
template <typename Value>
// NOLINTNEXTLINE(bugprone-std-namespace-modification,cert-dcl58-cpp)
struct std::formatter<Consolpp::Stream::PlainArgument<Value>, char>
{
    private:
        static constexpr std::size_t specificationCapacity = 32;

        std::formatter<Value, char> formatter;
        std::string_view specification;

    public:
        // Runs at compile time for the format of write(), so an unsupported specification does not compile.
        constexpr std::format_parse_context::iterator parse(std::format_parse_context& context)
        {
            const std::format_parse_context::iterator end = this->formatter.parse(context);

            this->specification = std::string_view{context.begin(), end};

            if (this->specification.find('{') != std::string_view::npos) {
                throw std::format_error{"Markup formats do not support nested replacement fields"};
            }

            if (this->specification.size() > specificationCapacity) {
                throw std::format_error{"The format specification is too long for a markup format"};
            }

            return end;
        }

        std::format_context::iterator format(const Consolpp::Stream::PlainArgument<Value>& argument,
                                             std::format_context& context) const
        {
            std::array<char, specificationCapacity + 3> field{'{', ':'};
            auto* const specificationEnd = std::ranges::copy(this->specification, std::next(field.begin(), 2)).out;

            *specificationEnd = '}';

            const std::string_view format{field.data(),
                                          static_cast<std::size_t>(std::distance(field.data(), specificationEnd) + 1)};

            return std::vformat_to(Consolpp::Markup::EncodingIterator{context.out()},
                                   format,
                                   std::make_format_args(*argument.value))
                .base();
        }
};
