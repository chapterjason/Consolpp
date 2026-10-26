#pragma once

#include "Consolpp/Markup/Color.hpp"

#include <optional>

namespace Consolpp::Markup
{
    struct Style
    {
        public:
            std::optional<Color> foreground{};
            std::optional<Color> background{};
            bool bold{false};
            bool underline{false};
            bool blink{false};
            bool reverse{false};
            bool conceal{false};

            static const Style error;
            static const Style warning;
            static const Style info;
            static const Style comment;
            static const Style question;

            constexpr bool operator==(const Style& other) const = default;
    };

    inline constexpr Style Style::error{.foreground = Color::white, .background = Color::red};
    inline constexpr Style Style::warning{.foreground = Color::black, .background = Color::yellow};
    inline constexpr Style Style::info{.foreground = Color::green};
    inline constexpr Style Style::comment{.foreground = Color::yellow};
    inline constexpr Style Style::question{.foreground = Color::black, .background = Color::cyan};
} // namespace Consolpp::Markup
