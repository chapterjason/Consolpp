#pragma once

#include "Consolpp/Prompt/Field.hpp"
#include "Consolpp/Prompt/FormField.hpp"

#include <array>
#include <span>
#include <tuple>

namespace Consolpp::Prompt
{
    // Fields asked together; the fields are copied, so the form can be asked again.
    template <Field... Fields>
        requires(sizeof...(Fields) > 0)
    class Form
    {
        private:
            std::tuple<Fields...> fields;
            std::array<FormField, sizeof...(Fields)> formFields;

        public:
            explicit Form(const Fields&... fields)
                : fields{fields...},
                  formFields{FormField{fields}...}
            {
            }

            [[nodiscard]]
            const std::tuple<Fields...>& getFields() const
            {
                return this->fields;
            }

            [[nodiscard]]
            std::span<const FormField> getFormFields() const
            {
                return this->formFields;
            }
    };
} // namespace Consolpp::Prompt
