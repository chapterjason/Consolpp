#pragma once

#include "Consolpp/Markup/Attribute.hpp"
#include "Consolpp/Markup/ColorAttributes.hpp"
#include "Consolpp/Markup/Decode.hpp"
#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/ElementError.hpp"
#include "Consolpp/Markup/ElementErrorKind.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/FlagAttributes.hpp"
#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/ParseColor.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"
#include "Consolpp/Text/InplaceString.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace Consolpp::Markup
{
    // Values other than href are compared after resolving entities; href stays escaped for the output.
    // The registered tags give the styles of the named elements; style and link are built in.
    inline ElementStyle resolveElement(const Markup::Element& element)
    {
        // Longer than every valid value, so a value that does not fit is invalid.
        constexpr std::size_t valueCapacity = 16;
        const auto createInvalidValue = [](const Markup::Attribute& attribute) {
            return ElementError{ElementErrorKind::InvalidAttributeValue, attribute.value, attribute.valuePosition};
        };
        const auto decodeValue = [&createInvalidValue](const Markup::Attribute& attribute) {
            Text::InplaceString<valueCapacity> value;

            try {
                Markup::decode(attribute.value, std::back_inserter(value));
            } catch (const std::length_error&) {
                throw createInvalidValue(attribute);
            }

            return value;
        };
        const auto parseColorAttribute = [&decodeValue, &createInvalidValue](const Markup::Attribute& attribute) {
            try {
                return Markup::parseColor(decodeValue(attribute).getView());
            } catch (const std::invalid_argument&) {
                throw createInvalidValue(attribute);
            }
        };
        const auto parseFlagAttribute = [&decodeValue, &createInvalidValue](const Markup::Attribute& attribute) {
            const auto value = decodeValue(attribute);

            if (value.getView() != "true" && value.getView() != "false") {
                throw createInvalidValue(attribute);
            }

            return value.getView() == "true";
        };
        const bool link = element.name == "link";
        const bool styleElement = link || element.name == "style";
        const std::vector<TagDefinition>& tags = Markup::getTags();
        const auto named = std::ranges::find(tags, element.name, &TagDefinition::name);
        ElementStyle resolved{};
        bool hasHref = false;

        if (!styleElement && named != tags.end()) {
            resolved.style = named->style;
        } else if (!styleElement) {
            throw ElementError{ElementErrorKind::UnknownElement, element.name, element.position + 1};
        }

        for (const Markup::Attribute& attribute : element.getAttributes()) {
            const auto* const color = std::ranges::find(Markup::colorAttributes,
                                                        attribute.name,
                                                        &decltype(Markup::colorAttributes)::value_type::first);
            const auto* const flag = std::ranges::find(Markup::flagAttributes,
                                                       attribute.name,
                                                       &decltype(Markup::flagAttributes)::value_type::first);

            if (styleElement && color != Markup::colorAttributes.end()) {
                resolved.style.*(color->second) = parseColorAttribute(attribute);
            } else if (styleElement && flag != Markup::flagAttributes.end()) {
                resolved.style.*(flag->second) = parseFlagAttribute(attribute);
            } else if (link && attribute.name == "href") {
                resolved.href = attribute.value;
                hasHref = true;
            } else {
                throw ElementError{ElementErrorKind::UnknownAttribute, attribute.name, attribute.namePosition};
            }
        }

        if (link && !hasHref) {
            throw ElementError{ElementErrorKind::MissingAttribute, "href", element.position};
        }

        return resolved;
    }
} // namespace Consolpp::Markup
