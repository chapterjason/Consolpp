#include "Consolpp/Event/Registration.hpp"

#include "Consolpp/Event/Loop.hpp"

namespace Consolpp::Event
{
    void Registration::attach()
    {
        this->loop->attach(*this);
    }

    Registration::~Registration()
    {
        this->loop->detach(*this);
    }
} // namespace Consolpp::Event
