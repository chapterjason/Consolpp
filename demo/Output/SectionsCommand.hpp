#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Screen/Section.hpp"
#include "Consolpp/Screen/SectionStack.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <chrono>
#include <cstddef>
#include <format>
#include <string>
#include <string_view>
#include <thread>

namespace Demo::Output
{
    class SectionsCommand
    {
        public:
            static constexpr Consolpp::Definition definition{
                .name = "output:sections",
                .description = "Updates sections of the output while files upload",
                .help = "The log keeps its last four lines, and the progress line is overwritten.",
                .signature = Consolpp::Signature{std::array<Consolpp::Option, 0>{},
                                                 std::array<Consolpp::Argument, 0>{}},
            };

            static int run(Consolpp::Stream::OutputStream& output)
            {
                struct File
                {
                    public:
                        std::string_view path;
                        std::size_t kilobytes;
                        bool excluded{false};
                };

                constexpr std::array<File, 8> files{
                    {
                        {.path = "config/app.yaml", .kilobytes = 4},
                        {.path = "public/index.php", .kilobytes = 2},
                        {.path = "public/assets/app.css", .kilobytes = 148},
                        {.path = "public/assets/app.js", .kilobytes = 612},
                        {.path = "var/cache/", .kilobytes = 0, .excluded = true},
                        {.path = "src/Kernel.php", .kilobytes = 9},
                        {.path = "src/Controller/HomeController.php", .kilobytes = 3},
                        {.path = "var/log/", .kilobytes = 0, .excluded = true},
                    },
                };
                constexpr std::size_t barWidth = 24;
                constexpr std::chrono::milliseconds uploadTime{350};
                Consolpp::Screen::SectionStack sections{output};
                Consolpp::Screen::Section& header = sections.createSection();
                Consolpp::Screen::Section& log = sections.createSection();
                Consolpp::Screen::Section& progress = sections.createSection();
                std::size_t uploaded = 0;

                header.overwrite("<comment>Deploying</comment> app.tar to <comment>production</comment>\n");
                log.setMaxHeight(4);

                for (std::size_t index = 0; index < files.size(); ++index) {
                    const File& file = files.at(index);
                    const std::size_t filled = (index + 1) * barWidth / files.size();

                    std::this_thread::sleep_for(uploadTime);

                    if (file.excluded) {
                        log.append(std::format("<comment>skipped</comment>  {} (excluded)", file.path));
                    } else {
                        log.append(std::format("<info>uploaded</info> {} ({} KB)", file.path, file.kilobytes));
                        uploaded += file.kilobytes;
                    }

                    progress.overwrite(std::format("[<info>{}</info>{}] {:>3}%  {}/{} files",
                                                   std::string(filled, '#'),
                                                   std::string(barWidth - filled, '.'),
                                                   (index + 1) * 100 / files.size(),
                                                   index + 1,
                                                   files.size()));
                }

                progress.overwrite(std::format("<info>Deployed</info> {} KB", uploaded));

                return 0;
            }
    };
} // namespace Demo::Output
