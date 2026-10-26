#include "Consolpp/Markup/ResolveElement.hpp"

#include "Consolpp/Markup/AddTag.hpp"
#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/ElementError.hpp"
#include "Consolpp/Markup/ElementErrorKind.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/RemoveTag.hpp"
#include "Consolpp/Markup/SetTags.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::addTag;
    using Consolpp::Markup::Color;
    using Consolpp::Markup::Element;
    using Consolpp::Markup::ElementError;
    using Consolpp::Markup::ElementErrorKind;
    using Consolpp::Markup::ElementStyle;
    using Consolpp::Markup::getTags;
    using Consolpp::Markup::removeTag;
    using Consolpp::Markup::resolveElement;
    using Consolpp::Markup::setTags;
    using Consolpp::Markup::Style;
    using Consolpp::Markup::TagDefinition;

    namespace
    {
        using CaughtError = std::optional<std::tuple<ElementErrorKind, std::string, std::size_t>>;

        class ResolveElementTest : public ::testing::Test
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

    TEST_F(ResolveElementTest, UsesTheInfoStyle)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "info",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 5,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{.style = Style::info};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, UsesTheCommentStyle)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "comment",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 8,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{.style = Style::comment};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, UsesTheQuestionStyle)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "question",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 9,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{.style = Style::question};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, UsesTheErrorStyle)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "error",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 6,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{.style = Style::error};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, UsesNoStyleForAStyleWithoutAttributes)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 6,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, ResolvesColorsAndFlags)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( foreground="cyan" background="#f80" bold="true" underline="false")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{
            .style = Style{.foreground = Color::cyan, .background = Color::fromHex("#f80"), .bold = true},
        };

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, ResolvesEveryFlag)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( bold="true" underline="true" blink="true" reverse="true" conceal="true")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{
            .style = Style{.bold = true, .underline = true, .blink = true, .reverse = true, .conceal = true},
        };

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, ResolvesColorsIgnoringCase)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( foreground="RED")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{.style = Style{.foreground = Color::red}};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, ResolvesEntitiesInValues)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( foreground="&#114;ed")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{.style = Style{.foreground = Color::red}};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, KeepsTheHrefEscaped)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "link",
            .position = 0,
            .attributes = R"( href="https://example.com/?a=1&amp;b=2")",
            .attributesPosition = 5,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{.href = "https://example.com/?a=1&amp;b=2"};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, ResolvesStyleAttributesOnALink)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "link",
            .position = 0,
            .attributes = R"( href="https://example.com" foreground="green")",
            .attributesPosition = 5,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{.style = Style{.foreground = Color::green}, .href = "https://example.com"};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, AcceptsAnEmptyHref)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "link",
            .position = 0,
            .attributes = R"( href="")",
            .attributesPosition = 5,
            .selfClosing = false,
        };
        constexpr ElementStyle expected{};

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved, expected);
    }

    TEST_F(ResolveElementTest, ThrowsForAnUnknownElement)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "bogus",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 6,
            .selfClosing = false,
        };
        const CaughtError expected{std::in_place, ElementErrorKind::UnknownElement, "bogus", 1};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(resolveElement(element));
        } catch (const ElementError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST_F(ResolveElementTest, ThrowsForAnAttributeOnABuiltInElement)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "info",
            .position = 0,
            .attributes = R"( foreground="red")",
            .attributesPosition = 5,
            .selfClosing = false,
        };
        const CaughtError expected{std::in_place, ElementErrorKind::UnknownAttribute, "foreground", 6};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(resolveElement(element));
        } catch (const ElementError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST_F(ResolveElementTest, ThrowsForAnHrefOnStyle)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( href="u")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        const CaughtError expected{std::in_place, ElementErrorKind::UnknownAttribute, "href", 7};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(resolveElement(element));
        } catch (const ElementError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST_F(ResolveElementTest, ThrowsForAnUnknownAttribute)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( size="1")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        const CaughtError expected{std::in_place, ElementErrorKind::UnknownAttribute, "size", 7};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(resolveElement(element));
        } catch (const ElementError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST_F(ResolveElementTest, ThrowsForALinkWithoutHref)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "link",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 5,
            .selfClosing = false,
        };
        const CaughtError expected{std::in_place, ElementErrorKind::MissingAttribute, "href", 0};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(resolveElement(element));
        } catch (const ElementError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST_F(ResolveElementTest, ThrowsForAnUnknownColor)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( foreground="purple")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        const CaughtError expected{std::in_place, ElementErrorKind::InvalidAttributeValue, "purple", 19};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(resolveElement(element));
        } catch (const ElementError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST_F(ResolveElementTest, ThrowsForAnInvalidFlag)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( bold="yes")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        const CaughtError expected{std::in_place, ElementErrorKind::InvalidAttributeValue, "yes", 13};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(resolveElement(element));
        } catch (const ElementError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST_F(ResolveElementTest, ThrowsForAValueLongerThanAnyValidOne)
    {
        // Arrange
        constexpr Element element = Element{
            .name = "style",
            .position = 0,
            .attributes = R"( bold="truetruetruetruetrue")",
            .attributesPosition = 6,
            .selfClosing = false,
        };
        const CaughtError expected{std::in_place, ElementErrorKind::InvalidAttributeValue, "truetruetruetruetrue", 13};
        CaughtError caught;

        // Act
        try {
            static_cast<void>(resolveElement(element));
        } catch (const ElementError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()}, error.getPosition());
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST_F(ResolveElementTest, UsesTheStyleOfARegisteredTag)
    {
        // Arrange
        constexpr Element element{
            .name = "deploy",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 7,
            .selfClosing = false,
        };

        addTag(TagDefinition{.name = "deploy", .style = Style{.bold = true}});

        // Act
        const ElementStyle resolved = resolveElement(element);

        // Assert
        EXPECT_EQ(resolved.style, Style{.bold = true});
    }

    TEST_F(ResolveElementTest, ThrowsForARemovedTag)
    {
        // Arrange
        constexpr Element element{
            .name = "info",
            .position = 0,
            .attributes = std::string_view{},
            .attributesPosition = 5,
            .selfClosing = false,
        };

        removeTag("info");

        // Act
        const auto resolve = [&element] {
            static_cast<void>(resolveElement(element));
        };

        // Assert
        EXPECT_THROW(resolve(), ElementError);
    }
} // namespace Consolpp::Tests::Unit::Markup
