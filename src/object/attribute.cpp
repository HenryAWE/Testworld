// Author: HenryAWE
// License: The 3-clause BSD License

#include "attribute.hpp"
#include <typeinfo>


namespace awe::attribute
{
    Base::Base() noexcept = default;
    Base::~Base() noexcept = default;

    const char* Base::GetName()
    {
        return typeid(this).name();
    }
}
