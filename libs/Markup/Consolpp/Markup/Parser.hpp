#pragma once

#include "Consolpp/Markup/AttributeRange.hpp"
#include "Consolpp/Markup/DecodeEntity.hpp"
#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/IsName.hpp"
#include "Consolpp/Markup/ParseError.hpp"
#include "Consolpp/Markup/ParseErrorKind.hpp"
#include "Consolpp/Text/IsWhitespace.hpp"

#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace Consolpp::Markup
{
    // Checks the syntax only; what an element means is up to the resolver.
    class Parser
    {
        private:
            std::string_view markup;
            std::size_t position{0};

            static constexpr void decodeEntities(const std::string_view text, const std::size_t textPosition)
            {
                for (std::size_t ampersand = text.find('&'); ampersand != std::string_view::npos;
                     ampersand = text.find('&', ampersand + 1)) {
                    static_cast<void>(Markup::decodeEntity(text.substr(ampersand), textPosition + ampersand));
                }
            }

            [[nodiscard]]
            constexpr bool hasCharacter(const char character) const
            {
                return this->position < this->markup.size() && this->markup.at(this->position) == character;
            }

            constexpr void skipWhitespace()
            {
                while (this->position < this->markup.size() && Text::isWhitespace(this->markup.at(this->position))) {
                    ++this->position;
                }
            }

            [[nodiscard]]
            ParseError createMalformedTagError(const std::size_t tagPosition) const
            {
                return ParseError{ParseErrorKind::MalformedTag,
                                  this->markup.substr(tagPosition, this->position - tagPosition),
                                  tagPosition};
            }

            constexpr std::string_view parseName(const std::size_t tagPosition)
            {
                const std::size_t start = this->position;

                while (this->position < this->markup.size()
                       && ((this->markup.at(this->position) >= 'a' && this->markup.at(this->position) <= 'z')
                           || this->markup.at(this->position) == '-')) {
                    ++this->position;
                }

                const std::string_view name = this->markup.substr(start, this->position - start);

                if (!Markup::isName(name)) {
                    throw this->createMalformedTagError(tagPosition);
                }

                return name;
            }

            constexpr void expect(const char character, const std::size_t tagPosition)
            {
                if (!this->hasCharacter(character)) {
                    throw this->createMalformedTagError(tagPosition);
                }

                ++this->position;
            }

            constexpr void parseAttributeValue(const std::size_t tagPosition)
            {
                if (!this->hasCharacter('"') && !this->hasCharacter('\'')) {
                    throw this->createMalformedTagError(tagPosition);
                }

                const char quote = this->markup.at(this->position);
                const std::size_t start = this->position + 1;
                const std::size_t end = this->markup.find(quote, start);

                if (end == std::string_view::npos) {
                    this->position = this->markup.size();

                    throw this->createMalformedTagError(tagPosition);
                }

                const std::string_view value = this->markup.substr(start, end - start);

                this->position = end + 1;

                if (value.find('<') != std::string_view::npos) {
                    throw this->createMalformedTagError(tagPosition);
                }

                Parser::decodeEntities(value, start);
            }

            // The attributes before it are already checked, so they can be read to find duplicates.
            constexpr void parseAttribute(const std::size_t attributesPosition, const std::size_t tagPosition)
            {
                const std::size_t attributePosition = this->position;
                const std::string_view name = this->parseName(tagPosition);
                const AttributeRange previous{
                    this->markup.substr(attributesPosition, attributePosition - attributesPosition),
                    attributesPosition,
                };

                this->skipWhitespace();
                this->expect('=', tagPosition);
                this->skipWhitespace();
                this->parseAttributeValue(tagPosition);

                for (const Attribute& attribute : previous) {
                    if (attribute.name == name) {
                        throw ParseError{ParseErrorKind::DuplicateAttribute, name, attributePosition};
                    }
                }
            }

            // Starts after the "<"; tagPosition points at it.
            constexpr Element parseOpeningTag(const std::size_t tagPosition)
            {
                const std::string_view name = this->parseName(tagPosition);
                const std::size_t attributesPosition = this->position;
                std::size_t attributesEnd = this->position;

                while (true) {
                    const std::size_t beforeWhitespace = this->position;

                    this->skipWhitespace();

                    if (this->hasCharacter('>') || this->hasCharacter('/')) {
                        break;
                    }

                    if (this->position == beforeWhitespace) {
                        throw this->createMalformedTagError(tagPosition);
                    }

                    this->parseAttribute(attributesPosition, tagPosition);
                    attributesEnd = this->position;
                }

                const bool selfClosing = this->hasCharacter('/');

                if (selfClosing) {
                    ++this->position;
                }

                this->expect('>', tagPosition);

                return Element{
                    .name = name,
                    .position = tagPosition,
                    .attributes = this->markup.substr(attributesPosition, attributesEnd - attributesPosition),
                    .attributesPosition = attributesPosition,
                    .selfClosing = selfClosing,
                };
            }

            // Starts after the "</"; tagPosition points at the "<".
            constexpr void parseClosingTag(const std::string_view expected, const std::size_t tagPosition)
            {
                const std::string_view name = this->parseName(tagPosition);

                this->skipWhitespace();
                this->expect('>', tagPosition);

                if (expected.empty()) {
                    throw ParseError{ParseErrorKind::UnexpectedClosingTag, name, tagPosition};
                }

                if (name != expected) {
                    throw ParseError{ParseErrorKind::MismatchedClosingTag, name, tagPosition};
                }
            }

            // The call stack holds the open elements, so nesting needs no heap.
            template <typename Context, typename Resolve, typename Visit>
            // NOLINTNEXTLINE(misc-no-recursion)
            constexpr void parseContent(const Context& context,
                                        Resolve& resolve,
                                        Visit& visit,
                                        const std::string_view name,
                                        const std::size_t namePosition)
            {
                while (true) {
                    const std::size_t textStart = this->position;
                    const std::size_t tagPosition = this->markup.find('<', textStart);
                    const std::size_t textEnd = tagPosition == std::string_view::npos ? this->markup.size()
                                                                                      : tagPosition;
                    const std::string_view text = this->markup.substr(textStart, textEnd - textStart);

                    Parser::decodeEntities(text, textStart);

                    if (!text.empty()) {
                        visit(text, context);
                    }

                    if (tagPosition == std::string_view::npos) {
                        if (!name.empty()) {
                            throw ParseError{ParseErrorKind::UnclosedElement, name, namePosition};
                        }

                        this->position = textEnd;

                        return;
                    }

                    this->position = tagPosition + 1;

                    if (this->hasCharacter('/')) {
                        ++this->position;
                        this->parseClosingTag(name, tagPosition);

                        return;
                    }

                    const Element element = this->parseOpeningTag(tagPosition);
                    const Context inner = resolve(element);

                    if (!element.selfClosing) {
                        this->parseContent(inner, resolve, visit, element.name, element.position);
                    }
                }
            }

        public:
            explicit constexpr Parser(const std::string_view markup)
                : markup{markup}
            {
            }

            // The resolver turns each element into the context of its content; the root context covers the rest.
            // The visitor gets every text between tags, still escaped, with the context of its innermost element.
            template <typename Context, typename Resolve, typename Visit>
                requires std::convertible_to<std::invoke_result_t<Resolve&, const Element&>, Context>
                         && std::invocable<Visit&, std::string_view, const Context&>
            constexpr void parse(const Context& root, Resolve resolve, Visit visit)
            {
                this->position = 0;

                this->parseContent(root, resolve, visit, std::string_view{}, 0);
            }
    };
} // namespace Consolpp::Markup
