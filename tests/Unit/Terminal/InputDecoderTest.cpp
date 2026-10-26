#include "Consolpp/Terminal/InputDecoder.hpp"

#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Keyboard/Modifiers.hpp"

#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Terminal
{
    using Consolpp::Keyboard::Key;
    using Consolpp::Keyboard::KeyCode;
    using Consolpp::Keyboard::Modifiers;
    using Consolpp::Terminal::InputDecoder;

    namespace
    {
        const auto ignoreText = [](const std::string_view /*text*/) {
        };
    } // namespace

    TEST(InputDecoderTest, DecodesAnAsciiCharacter)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Character, .character = U'a'}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"a"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesAUtf8Character)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Character, .character = U'\u00E4'}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\xC3\xA4"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesAUtf8CharacterSplitOverTwoReads)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Character, .character = U'\u00E4'}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\xC3"}, collect, ignoreText);
        decoder.decode(std::string_view{"\xA4"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesEnterTabAndBackspace)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{
            Key{.code = KeyCode::Enter},
            Key{.code = KeyCode::Tab},
            Key{.code = KeyCode::Backspace},
        };
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\r\t\x7f"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesControlWithALetter)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{
            Key{.code = KeyCode::Character, .character = U'a', .modifiers = Modifiers{.control = true}},
        };
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x01"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesTheArrows)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{
            Key{.code = KeyCode::Up},
            Key{.code = KeyCode::Down},
            Key{.code = KeyCode::Right},
            Key{.code = KeyCode::Left},
        };
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b[A\x1b[B\x1b[C\x1b[D"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesModifiersOfASequence)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Right, .modifiers = Modifiers{.control = true}}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b[1;5C"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesAThreeByteSequence)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::F1}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1bOP"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesTildeSequences)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Delete}, Key{.code = KeyCode::F12}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b[3~\x1b[24~"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesModifiersOfATildeSequence)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::PageUp, .modifiers = Modifiers{.alt = true}}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b[5;3~"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesShiftTab)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Tab, .modifiers = Modifiers{.shift = true}}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b[Z"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesAltWithACharacter)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{
            Key{.code = KeyCode::Character, .character = U'x', .modifiers = Modifiers{.alt = true}},
        };
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1bx"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DecodesASequenceSplitOverTwoReads)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Up}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b["}, collect, ignoreText);
        decoder.decode(std::string_view{"A"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DropsAnUnknownSequenceAndGoesOn)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Character, .character = U'a'}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b[99~a"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, ReplacesAnInvalidByte)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Character, .character = U'\uFFFD'}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\xFF"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, WaitsWithALoneEscape)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, FlushTurnsALoneEscapeIntoTheEscapeKey)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Escape}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        decoder.decode(std::string_view{"\x1b"}, collect, ignoreText);

        // Act
        decoder.flush(collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
        EXPECT_FALSE(decoder.isPending());
    }

    TEST(InputDecoderTest, FlushDropsAnUnfinishedSequence)
    {
        // Arrange
        InputDecoder decoder;
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        decoder.decode(std::string_view{"\x1b[1;"}, collect, ignoreText);

        // Act
        decoder.flush(collect, ignoreText);

        // Assert
        EXPECT_TRUE(keys.empty());
    }

    TEST(InputDecoderTest, IsPendingWhileASequenceIsUnfinished)
    {
        // Arrange
        InputDecoder decoder;
        const auto ignore = [](const Key& /*key*/) {
        };

        // Act
        decoder.decode(std::string_view{"\x1b[1"}, ignore, ignoreText);

        // Assert
        EXPECT_TRUE(decoder.isPending());
    }

    TEST(InputDecoderTest, DecodesBackspaceAsControlBackspace)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{Key{.code = KeyCode::Backspace, .modifiers = Modifiers{.control = true}}};
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\b"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
    }

    TEST(InputDecoderTest, DeliversABracketedPasteWhole)
    {
        // Arrange
        InputDecoder decoder;
        std::vector<Key> keys;
        std::string pasted;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };
        const auto collectPaste = [&pasted](const std::string_view text) {
            pasted = text;
        };

        // Act
        decoder.decode(std::string_view{"\x1b[200~a\r\x1b[Ab\x1b[201~"}, collect, collectPaste);

        // Assert
        EXPECT_EQ(pasted, "a\r\x1b[Ab");
        EXPECT_TRUE(keys.empty());
    }

    TEST(InputDecoderTest, WaitsForTheEndOfAPasteSplitOverReads)
    {
        // Arrange
        InputDecoder decoder;
        std::string pasted;
        const auto ignore = [](const Key& /*key*/) {
        };
        const auto collectPaste = [&pasted](const std::string_view text) {
            pasted = text;
        };

        decoder.decode(std::string_view{"\x1b[200~ab\x1b[20"}, ignore, collectPaste);

        // Act
        decoder.decode(std::string_view{"1~"}, ignore, collectPaste);

        // Assert
        EXPECT_EQ(pasted, "ab");
    }

    TEST(InputDecoderTest, DecodesEscapeBracketAsAltBracket)
    {
        // Arrange
        InputDecoder decoder;
        const std::vector<Key> expected{
            Key{.code = KeyCode::Character, .character = U']', .modifiers = Modifiers{.alt = true}},
            Key{.code = KeyCode::Character, .character = U'x'},
        };
        std::vector<Key> keys;
        const auto collect = [&keys](const Key& key) {
            keys.push_back(key);
        };

        // Act
        decoder.decode(std::string_view{"\x1b]x"}, collect, ignoreText);

        // Assert
        EXPECT_EQ(keys, expected);
        EXPECT_FALSE(decoder.isPending());
    }
} // namespace Consolpp::Tests::Unit::Terminal
