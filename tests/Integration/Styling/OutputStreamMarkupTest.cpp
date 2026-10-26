#include "../Stream/Channel.hpp"
#include "../Stream/PacketChannel.hpp"
#include "Consolpp/Markup/AddTag.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/ParseError.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Markup/SetTags.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"
#include "Consolpp/Styling/StyledSpan.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"
#include "Consolpp/Terminal/ColorMode.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Styling
{
    using Consolpp::Markup::addTag;
    using Consolpp::Markup::ElementStyle;
    using Consolpp::Markup::getTags;
    using Consolpp::Markup::ParsedMarkup;
    using Consolpp::Markup::ParseError;
    using Consolpp::Markup::setTags;
    using Consolpp::Markup::Style;
    using Consolpp::Markup::TagDefinition;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Styling::parseMarkup;
    using Consolpp::Styling::StyledSpan;
    using Consolpp::Terminal::Capabilities;
    using Consolpp::Terminal::ColorMode;
    using Integration::Stream::Channel;
    using Integration::Stream::PacketChannel;

    namespace
    {
        class OutputStreamMarkupTest : public ::testing::Test
        {
            private:
                std::vector<TagDefinition> tags{getTags().begin(), getTags().end()};

            protected:
                void TearDown() override
                {
                    setTags(this->tags);
                }
        };
    } // namespace

    TEST_F(OutputStreamMarkupTest, WriteRendersMarkupWithoutStylesWhenUndecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};

        // Act
        output.write("<info>Deployed</info> {} files", 3);

        // Assert
        EXPECT_EQ(channel.drain(), "Deployed 3 files");
    }

    TEST_F(OutputStreamMarkupTest, WriteRendersMarkupWithStylesWhenDecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};

        // Act
        output.write("<info>Deployed</info> {} files", 3);

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[32mDeployed\x1b[39m 3 files");
    }

    TEST_F(OutputStreamMarkupTest, WriteInsertsArgumentsAsPlainText)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};

        // Act
        output.write("<info>{}</info>", "<b>&amp;");

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[32m<b>&amp;\x1b[39m");
    }

    TEST_F(OutputStreamMarkupTest, WriteUsesTheColorModeOfTheCapabilities)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(),
                            FlushMode::Immediate,
                            true,
                            Terminal::Capabilities{.colorMode = Terminal::ColorMode::TrueColor}};

        // Act
        output.write(R"(<style foreground="#ff8001">x</style>)");

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[38;2;255;128;1mx\x1b[39m");
    }

    TEST_F(OutputStreamMarkupTest, WriteKeepsBracesFromEntitiesLiteral)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};

        // Act
        output.write("&#123;{}&#125;", 1);

        // Assert
        EXPECT_EQ(channel.drain(), "{1}");
    }

    TEST_F(OutputStreamMarkupTest, WriteAppliesFormatSpecifications)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};
        constexpr int value = 7;

        // Act
        output.write("[{:>3}]", value);

        // Assert
        EXPECT_EQ(channel.drain(), "[  7]");
    }

    TEST_F(OutputStreamMarkupTest, WriteFormatsEveryKindOfString)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};
        const std::string text{"string"};
        constexpr std::string_view view = "view";

        // Act
        output.write("{} {} {}", "literal", text, view);

        // Assert
        EXPECT_EQ(channel.drain(), "literal string view");
    }

    TEST_F(OutputStreamMarkupTest, WriteFormatsFloatingPointNumbersAndBooleans)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};
        constexpr double value = 1.5;

        // Act
        output.write("{} {}", value, false);

        // Assert
        EXPECT_EQ(channel.drain(), "1.5 false");
    }

    TEST_F(OutputStreamMarkupTest, WriteUsesManualArgumentIndexes)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};

        // Act
        output.write("{1}{0}", 'a', 'b');

        // Assert
        EXPECT_EQ(channel.drain(), "ba");
    }

    TEST_F(OutputStreamMarkupTest, WriteFormatsOtherTypesWithStdFormat)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};
        const void* const pointer = nullptr;

        // Act
        output.write("{}", pointer);

        // Assert
        EXPECT_EQ(channel.drain(), "0x0");
    }

    TEST_F(OutputStreamMarkupTest, ShiftRendersMarkupWithStylesWhenDecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};
        constexpr std::string_view markup = "<comment>a &amp; b</comment>";

        // Act
        output << markup;

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[33ma & b\x1b[39m");
    }

    TEST_F(OutputStreamMarkupTest, ShiftRendersMarkupWithoutStylesWhenUndecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};
        constexpr std::string_view markup = "<comment>a &amp; b</comment>";

        // Act
        output << markup;

        // Assert
        EXPECT_EQ(channel.drain(), "a & b");
    }

    TEST_F(OutputStreamMarkupTest, ShiftWritesNothingForInvalidMarkup)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};
        constexpr std::string_view markup = "fine <info>broken";
        bool thrown = false;

        // Act
        try {
            output << markup;
        } catch (const ParseError&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
        EXPECT_EQ(channel.drain(), "");
    }

    TEST_F(OutputStreamMarkupTest, WritesAStyledSpanWithItsStyleWhenDecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};

        // Act
        output << StyledSpan{.style = Style::info, .text = "a&amp;b"};

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[32ma&amp;b\x1b[39m");
    }

    TEST_F(OutputStreamMarkupTest, WritesTheTextOfAStyledSpanWhenUndecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};

        // Act
        output << StyledSpan{.style = Style::info, .text = "ok"};

        // Assert
        EXPECT_EQ(channel.drain(), "ok");
    }

    TEST_F(OutputStreamMarkupTest, WritesParsedMarkupWithItsStylesWhenDecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};
        const ParsedMarkup<ElementStyle> markup = parseMarkup("a<info>b</info>");

        // Act
        output << markup;

        // Assert
        EXPECT_EQ(channel.drain(), "a\x1b[32mb\x1b[39m");
    }

    TEST_F(OutputStreamMarkupTest, WritesTheTextOfParsedMarkupWhenUndecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Terminal::Capabilities{}};
        const ParsedMarkup<ElementStyle> markup = parseMarkup("a<info>b</info>");

        // Act
        output << markup;

        // Assert
        EXPECT_EQ(channel.drain(), "ab");
    }

    TEST_F(OutputStreamMarkupTest, WritesASliceWithItsStylesClosedInsideWhenDecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Capabilities{}};
        const ParsedMarkup<ElementStyle> markup = parseMarkup("<info>aaa bbb</info>");
        constexpr std::size_t start = 4;
        constexpr std::size_t end = 7;

        // Act
        output << markup.getSlice(start, end);

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[32mbbb\x1b[39m");
    }

    TEST_F(OutputStreamMarkupTest, WritesTheTextOfASliceWhenUndecorated)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        const ParsedMarkup<ElementStyle> markup = parseMarkup("<info>aaa bbb</info>");
        constexpr std::size_t start = 4;
        constexpr std::size_t end = 7;

        // Act
        output << markup.getSlice(start, end);

        // Assert
        EXPECT_EQ(channel.drain(), "bbb");
    }

    TEST_F(OutputStreamMarkupTest, IsDecoratedReportsTheDecoration)
    {
        // Arrange
        const Channel channel;
        const OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};

        // Act
        const bool decorated = output.isDecorated();

        // Assert
        EXPECT_TRUE(decorated);
    }

    TEST_F(OutputStreamMarkupTest, PassesPlainValuesToTheWriter)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};
        constexpr int count = 5;

        // Act
        output << "n=" << count << ' ' << true;

        // Assert
        EXPECT_EQ(channel.drain(), "n=5 true");
    }

    TEST_F(OutputStreamMarkupTest, WriteSendsTheRenderedMarkupInOneWriteInImmediateMode)
    {
        // Arrange
        const PacketChannel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};
        const std::vector<std::string> expected{"\x1b[32mDeployed\x1b[39m 3 files"};

        // Act
        output.write("<info>Deployed</info> {} files", 3);

        // Assert
        EXPECT_EQ(channel.drainPackets(), expected);
    }

    TEST_F(OutputStreamMarkupTest, ShiftSendsTheRenderedMarkupInOneWriteInImmediateMode)
    {
        // Arrange
        const PacketChannel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};
        const std::vector<std::string> expected{"\x1b[32mDeployed\x1b[39m"};

        // Act
        output.write("<info>Deployed</info>");

        // Assert
        EXPECT_EQ(channel.drainPackets(), expected);
    }

    TEST_F(OutputStreamMarkupTest, WritesParsedMarkupInOneWriteInImmediateMode)
    {
        // Arrange
        const PacketChannel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Terminal::Capabilities{}};
        const ParsedMarkup<ElementStyle> markup = parseMarkup("<info>Deployed</info>");
        const std::vector<std::string> expected{"\x1b[32mDeployed\x1b[39m"};

        // Act
        output << markup;

        // Assert
        EXPECT_EQ(channel.drainPackets(), expected);
    }

    TEST_F(OutputStreamMarkupTest, EnablesMarkupByDefault)
    {
        // Arrange
        const Channel channel;

        // Act
        const OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        // Assert
        EXPECT_TRUE(output.isMarkupEnabled());
    }

    TEST_F(OutputStreamMarkupTest, WriteWritesTheTextAsItIsWhenMarkupIsDisabled)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        output.disableMarkup();

        // Act
        output.write("a <b & c");

        // Assert
        EXPECT_EQ(channel.drain(), "a <b & c");
    }

    TEST_F(OutputStreamMarkupTest, WriteReadsMarkupAgainAfterEnablingIt)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        output.disableMarkup();
        output.enableMarkup();

        // Act
        output.write("<info>a</info> &amp; b");

        // Assert
        EXPECT_EQ(channel.drain(), "a & b");
    }

    TEST_F(OutputStreamMarkupTest, WriteRawWritesMarkupAsItIsWhileMarkupIsEnabled)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Capabilities{}};

        // Act
        output.writeRaw("<info>a</info> & b");

        // Assert
        EXPECT_EQ(channel.drain(), "<info>a</info> & b");
    }

    TEST_F(OutputStreamMarkupTest, WriteKeepsALessThanSignInAFormatSpecificationOutOfTheMarkup)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        // Act
        output.write("<info>{:<4}</info>|", "ab");

        // Assert
        EXPECT_EQ(channel.drain(), "ab  |");
    }

    TEST_F(OutputStreamMarkupTest, WriteFormatsWithoutReadingMarkupWhenMarkupIsDisabled)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        output.disableMarkup();

        // Act
        output.write("<info>{}</info>", "&");

        // Assert
        EXPECT_EQ(channel.drain(), "<info>&</info>");
    }

    TEST_F(OutputStreamMarkupTest, WriteUsesTheRegisteredTags)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, true, Capabilities{}};

        addTag(TagDefinition{.name = "deploy", .style = Style{.bold = true}});

        // Act
        output.write("<deploy>{}</deploy>", 3);

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[1m3\x1b[22m");
    }
} // namespace Consolpp::Tests::Integration::Styling
