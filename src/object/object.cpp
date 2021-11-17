// Author: HenryAWE
// License: The 3-clause BSD License

#include "object.hpp"


namespace awe
{
    Object::Object() = default;
    Object::~Object() = default;

    attribute::Base& Object::GetAttrByIdx(std::size_t index)
    {
        return *m_attr[index];
    }
}
