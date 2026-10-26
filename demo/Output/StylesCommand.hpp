#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>

namespace Demo::Output
{
    class StylesCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "output:styles",
                .description = "Writes the ready-made tags, colors, text attributes, links and a custom tag",
                .help = "The styles adapt to the terminal: colors are reduced or left out, e.g. with NO_COLOR or in "
                        "a pipe.",
                .signature = Consolpp::Signature{std::array<Consolpp::Option, 0>{},
                                                 std::array<Consolpp::Argument, 0>{}},
            };

            static int run(Consolpp::Stream::OutputStream& output)
            {
                output.write("<comment>Ready-made tags</comment>\n");
                output.write("  <info>info</info>  <comment>comment</comment>  <question>question</question>  "
                             "<error>error</error>  <warning>warning</warning>\n");
                output.write("<comment>Named colors</comment>\n");
                output.write("  <style foreground=\"red\">red</style>  <style foreground=\"green\">green</style>  "
                             "<style foreground=\"blue\">blue</style>  "
                             "<style foreground=\"bright-magenta\">bright-magenta</style>  "
                             "<style foreground=\"black\" background=\"bright-yellow\">on bright-yellow</style>\n");
                output.write("<comment>Hex colors</comment>\n");
                output.write("  <style foreground=\"#ff8800\">#ff8800</style>  "
                             "<style foreground=\"#08f\">#08f</style>  "
                             "<style background=\"#5f2a7a\">background #5f2a7a</style>\n");
                output.write("<comment>Attributes</comment>\n");
                output.write("  <style bold=\"true\">bold</style>  <style underline=\"true\">underline</style>  "
                             "<style reverse=\"true\">reverse</style>  <style blink=\"true\">blink</style>  "
                             "<style conceal=\"true\">conceal</style> (concealed)\n");
                output.write("<comment>Link</comment>\n");
                output.write("  <link href=\"https://github.com/chapterjason/Consolpp\">Consolpp on GitHub</link>\n");
                output.write("<comment>Custom tag</comment>\n");
                output.write("  <highlight>highlight</highlight>, defined by the demo with Markup::addTag()\n");
                output.write("<comment>Escaped markup</comment>\n");
                output.write("  &lt;info&gt; is written as text, and so are values: {}\n", "<info>not styled</info>");

                return 0;
            }
    };
} // namespace Demo::Output
