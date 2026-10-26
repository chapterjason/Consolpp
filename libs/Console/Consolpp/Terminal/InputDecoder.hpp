#pragma once

#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Keyboard/Modifiers.hpp"
#include "Consolpp/Terminal/DecoderMode.hpp"
#include "Consolpp/Text/Utf8/CodePoint.hpp"
#include "Consolpp/Text/Utf8/Decode.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace Consolpp::Terminal
{
    // Turns the bytes a terminal sends into keys and bracketed pastes. A sequence split over several reads waits for
    // its rest; a lone ESC waits as well, since it may start a sequence, until flush() is called after a short timeout.
    class InputDecoder
    {
        private:
            // Longer than every sequence that is decoded, so a sequence that does not fit is dropped.
            static constexpr std::size_t capacity = 32;
            static constexpr char escape = '\x1b';
            static constexpr char32_t replacement = 0xFFFD;
            static constexpr std::uint32_t pasteStart = 200;
            static constexpr std::string_view pasteEnd{"\x1b[201~"};

            // The length of what was decoded, or nothing when the bytes end before the key does.
            struct Decoded
            {
                public:
                    std::optional<Keyboard::Key> key;
                    std::size_t length;
                    DecoderMode mode{DecoderMode::Keys};
            };

            std::array<char, capacity> pending{};
            std::size_t pendingLength{0};
            DecoderMode mode{DecoderMode::Keys};
            std::string collected;

            static constexpr Keyboard::Modifiers decodeModifiers(const std::uint32_t parameter)
            {
                const std::uint32_t bits = parameter > 0 ? parameter - 1 : 0;

                return Keyboard::Modifiers{
                    .shift = (bits & 1U) != 0,
                    .alt = (bits & 2U) != 0,
                    .control = (bits & 4U) != 0,
                };
            }

            static constexpr std::optional<Keyboard::KeyCode> decodeTilde(const std::uint32_t number)
            {
                // The numbers of "ESC [ number ~" as xterm, VT220 and the Linux console send them.
                constexpr std::array<std::pair<std::uint32_t, Keyboard::KeyCode>, 20> codes{
                    {
                        {1, Keyboard::KeyCode::Home}, {2, Keyboard::KeyCode::Insert}, {3, Keyboard::KeyCode::Delete},
                        {4, Keyboard::KeyCode::End},  {5, Keyboard::KeyCode::PageUp}, {6, Keyboard::KeyCode::PageDown},
                        {7, Keyboard::KeyCode::Home}, {8, Keyboard::KeyCode::End},    {11, Keyboard::KeyCode::F1},
                        {12, Keyboard::KeyCode::F2},  {13, Keyboard::KeyCode::F3},    {14, Keyboard::KeyCode::F4},
                        {15, Keyboard::KeyCode::F5},  {17, Keyboard::KeyCode::F6},    {18, Keyboard::KeyCode::F7},
                        {19, Keyboard::KeyCode::F8},  {20, Keyboard::KeyCode::F9},    {21, Keyboard::KeyCode::F10},
                        {23, Keyboard::KeyCode::F11}, {24, Keyboard::KeyCode::F12},
                    },
                };

                const auto* const found = std::ranges::find(codes,
                                                            number,
                                                            &std::pair<std::uint32_t, Keyboard::KeyCode>::first);

                return found == codes.end() ? std::nullopt : std::optional<Keyboard::KeyCode>{found->second};
            }

            // The final byte of a CSI or SS3 sequence without parameters.
            static constexpr std::optional<Keyboard::KeyCode> decodeFinal(const char final)
            {
                switch (final) {
                    case 'A':
                        return Keyboard::KeyCode::Up;
                    case 'B':
                        return Keyboard::KeyCode::Down;
                    case 'C':
                        return Keyboard::KeyCode::Right;
                    case 'D':
                        return Keyboard::KeyCode::Left;
                    case 'H':
                        return Keyboard::KeyCode::Home;
                    case 'F':
                        return Keyboard::KeyCode::End;
                    case 'P':
                        return Keyboard::KeyCode::F1;
                    case 'Q':
                        return Keyboard::KeyCode::F2;
                    case 'R':
                        return Keyboard::KeyCode::F3;
                    case 'S':
                        return Keyboard::KeyCode::F4;
                    default:
                        return std::nullopt;
                }
            }

            // ESC [ parameters final; the bytes start after the "[".
            static constexpr Decoded decodeControlSequence(const std::string_view bytes)
            {
                std::array<std::uint32_t, 2> parameters{};
                std::size_t count = 0;
                std::size_t index = 0;
                constexpr std::uint32_t decimalBase = 10;
                constexpr std::uint32_t parameterLimit = 1000;

                while (index < bytes.size()
                       && ((bytes.at(index) >= '0' && bytes.at(index) <= '9') || bytes.at(index) == ';')) {
                    const char byte = bytes.at(index);

                    if (byte == ';') {
                        ++count;
                    } else if (count < parameters.size()) {
                        std::uint32_t& parameter = parameters.at(count);

                        parameter = std::min((parameter * decimalBase) + static_cast<std::uint32_t>(byte - '0'),
                                             parameterLimit);
                    }

                    ++index;
                }

                if (index == bytes.size()) {
                    return Decoded{.key = std::nullopt, .length = 0};
                }

                const char final = bytes.at(index);
                const std::size_t length = index + 3;
                const Keyboard::Modifiers modifiers = InputDecoder::decodeModifiers(std::get<1>(parameters));

                if (final == '~' && count == 0 && std::get<0>(parameters) == pasteStart) {
                    return Decoded{.key = std::nullopt, .length = length, .mode = DecoderMode::Paste};
                }

                if (final == 'Z') {
                    return Decoded{
                        .key =
                            Keyboard::Key{
                                .code = Keyboard::KeyCode::Tab,
                                .modifiers = Keyboard::Modifiers{.shift = true},
                            },
                        .length = length,
                    };
                }

                const std::optional<Keyboard::KeyCode> code = final == '~'
                                                                  ? InputDecoder::decodeTilde(std::get<0>(parameters))
                                                                  : InputDecoder::decodeFinal(final);

                return Decoded{
                    .key = code.has_value()
                               ? std::optional<Keyboard::Key>{Keyboard::Key{.code = *code, .modifiers = modifiers}}
                               : std::nullopt,
                    .length = length,
                };
            }

            // Ctrl with a letter sends the letter's position in the alphabet; Ctrl+Space sends NUL, and 0x1C to 0x1F
            // stand for the characters 0x40 above them.
            static constexpr char32_t decodeControlCharacter(const char byte)
            {
                constexpr char lastLetter = 0x1a;

                if (byte == 0) {
                    return U' ';
                }

                if (byte <= lastLetter) {
                    return static_cast<char32_t>(byte + 'a' - 1);
                }

                return static_cast<char32_t>(byte + '@');
            }

            // From the lead byte as RFC 3629 allows it; any other byte counts alone, and decode() rejects it.
            static constexpr std::size_t getUtf8Length(const char lead)
            {
                constexpr unsigned char firstTwoBytes = 0xC2;
                constexpr unsigned char firstThreeBytes = 0xE0;
                constexpr unsigned char firstFourBytes = 0xF0;
                constexpr unsigned char lastFourBytes = 0xF4;
                const auto byte = static_cast<unsigned char>(lead);

                if (byte >= firstFourBytes && byte <= lastFourBytes) {
                    return 4;
                }

                if (byte >= firstThreeBytes && byte < firstFourBytes) {
                    return 3;
                }

                return byte >= firstTwoBytes && byte < firstThreeBytes ? 2 : 1;
            }

            // A key without ESC in front: a control character, DEL or a UTF-8 character.
            static constexpr Decoded decodePlain(const std::string_view bytes)
            {
                constexpr char firstPrintable = 0x20;
                constexpr char deleteCharacter = 0x7f;
                const char byte = bytes.front();

                if (byte == '\r' || byte == '\n') {
                    return Decoded{.key = Keyboard::Key{.code = Keyboard::KeyCode::Enter}, .length = 1};
                }

                if (byte == '\t') {
                    return Decoded{.key = Keyboard::Key{.code = Keyboard::KeyCode::Tab}, .length = 1};
                }

                if (byte == deleteCharacter) {
                    return Decoded{.key = Keyboard::Key{.code = Keyboard::KeyCode::Backspace}, .length = 1};
                }

                // Terminals send DEL for Backspace and BS for Ctrl+Backspace.
                if (byte == '\b') {
                    return Decoded{
                        .key =
                            Keyboard::Key{
                                .code = Keyboard::KeyCode::Backspace,
                                .modifiers = Keyboard::Modifiers{.control = true},
                            },
                        .length = 1,
                    };
                }

                if (byte >= 0 && byte < firstPrintable) {
                    return Decoded{
                        .key =
                            Keyboard::Key{
                                .code = Keyboard::KeyCode::Character,
                                .character = InputDecoder::decodeControlCharacter(byte),
                                .modifiers = Keyboard::Modifiers{.control = true},
                            },
                        .length = 1,
                    };
                }

                const std::size_t length = InputDecoder::getUtf8Length(byte);

                if (bytes.size() < length) {
                    return Decoded{.key = std::nullopt, .length = 0};
                }

                try {
                    const Text::Utf8::CodePoint point = Text::Utf8::decode(bytes.substr(0, length));

                    return Decoded{
                        .key = Keyboard::Key{.code = Keyboard::KeyCode::Character, .character = point.value},
                        .length = point.length,
                    };
                } catch (const std::invalid_argument&) {
                    return Decoded{
                        .key = Keyboard::Key{.code = Keyboard::KeyCode::Character, .character = replacement},
                        .length = 1,
                    };
                }
            }

            static constexpr Decoded decodeOne(const std::string_view bytes)
            {
                if (bytes.front() != escape) {
                    return InputDecoder::decodePlain(bytes);
                }

                if (bytes.size() == 1) {
                    return Decoded{.key = std::nullopt, .length = 0};
                }

                if (bytes.at(1) == '[') {
                    return InputDecoder::decodeControlSequence(bytes.substr(2));
                }

                if (bytes.at(1) == 'O') {
                    if (bytes.size() < 3) {
                        return Decoded{.key = std::nullopt, .length = 0};
                    }

                    const std::optional<Keyboard::KeyCode> code = InputDecoder::decodeFinal(bytes.at(2));

                    return Decoded{
                        .key = code.has_value() ? std::optional<Keyboard::Key>{Keyboard::Key{.code = *code}}
                                                : std::nullopt,
                        .length = 3,
                    };
                }

                if (bytes.at(1) == escape) {
                    return Decoded{
                        .key =
                            Keyboard::Key{
                                .code = Keyboard::KeyCode::Escape,
                                .modifiers = Keyboard::Modifiers{.alt = true},
                            },
                        .length = 2,
                    };
                }

                Decoded decoded = InputDecoder::decodePlain(bytes.substr(1));

                if (!decoded.key.has_value()) {
                    return decoded;
                }

                decoded.key->modifiers.alt = true;
                ++decoded.length;

                return decoded;
            }

            template <std::invocable<std::string_view> OnPaste>
            constexpr void collect(const char byte, OnPaste& onPaste)
            {
                this->collected += byte;

                if (this->collected.ends_with(pasteEnd)) {
                    this->finishCollecting(onPaste, pasteEnd.size());
                }
            }

            template <std::invocable<std::string_view> OnPaste>
            constexpr void finishCollecting(OnPaste& onPaste, const std::size_t terminatorLength)
            {
                this->collected.resize(this->collected.size() - terminatorLength);
                this->mode = DecoderMode::Keys;
                onPaste(std::string_view{this->collected});
                this->collected.clear();
            }

            template <std::invocable<std::string_view> OnPaste>
            constexpr void collectPending(OnPaste& onPaste)
            {
                std::size_t taken = 0;

                while (taken < this->pendingLength && this->mode != DecoderMode::Keys) {
                    this->collect(this->pending.at(taken), onPaste);
                    ++taken;
                }

                std::ranges::copy(std::span{this->pending}.subspan(taken, this->pendingLength - taken),
                                  this->pending.begin());
                this->pendingLength -= taken;
            }

            template <std::invocable<const Keyboard::Key&> OnKey, std::invocable<std::string_view> OnPaste>
            constexpr void decodeByte(const char byte, OnKey& onKey, OnPaste& onPaste)
            {
                if (this->mode != DecoderMode::Keys) {
                    this->collect(byte, onPaste);

                    return;
                }

                if (this->pendingLength == capacity) {
                    this->pendingLength = 0;
                }

                this->pending.at(this->pendingLength) = byte;
                ++this->pendingLength;

                while (this->pendingLength > 0) {
                    if (this->mode != DecoderMode::Keys) {
                        this->collectPending(onPaste);

                        continue;
                    }

                    const Decoded decoded = InputDecoder::decodeOne(
                        std::string_view{this->pending.data(), this->pendingLength});

                    if (decoded.length == 0) {
                        return;
                    }

                    if (decoded.key.has_value()) {
                        onKey(*decoded.key);
                    }

                    std::ranges::copy(
                        std::span{this->pending}.subspan(decoded.length, this->pendingLength - decoded.length),
                        this->pending.begin());
                    this->pendingLength -= decoded.length;
                    this->mode = decoded.mode;
                }
            }

        public:
            // The view passed to onPaste ends with the call.
            template <std::invocable<const Keyboard::Key&> OnKey, std::invocable<std::string_view> OnPaste>
            constexpr void decode(const std::span<const char> bytes, OnKey onKey, OnPaste onPaste)
            {
                for (const char byte : bytes) {
                    this->decodeByte(byte, onKey, onPaste);
                }
            }

            // Whether bytes wait for the rest of a key, e.g. a lone ESC.
            [[nodiscard]]
            constexpr bool isPending() const
            {
                return this->pendingLength > 0;
            }

            // Ends the waiting after the timeout: a lone ESC is the Escape key, ESC with an unfinished sequence is
            // dropped, and the start of a UTF-8 character is the replacement character.
            template <std::invocable<const Keyboard::Key&> OnKey, std::invocable<std::string_view> OnPaste>
            constexpr void flush(OnKey onKey, OnPaste onPaste)
            {
                if (this->mode == DecoderMode::Paste) {
                    this->finishCollecting(onPaste, 0);
                }

                while (this->isPending()) {
                    const std::string_view bytes{this->pending.data(), this->pendingLength};

                    if (bytes == std::string_view{&escape, 1}) {
                        onKey(Keyboard::Key{.code = Keyboard::KeyCode::Escape});
                    } else if (bytes.front() != escape) {
                        onKey(Keyboard::Key{.code = Keyboard::KeyCode::Character, .character = replacement});
                    }

                    this->pendingLength = 0;
                }
            }
    };
} // namespace Consolpp::Terminal
