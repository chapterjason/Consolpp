#pragma once

namespace Consolpp::Event
{
    class Registration;

    struct DescriptorRegistrations
    {
        public:
            Registration* readable{nullptr};
            Registration* writable{nullptr};
    };
} // namespace Consolpp::Event
