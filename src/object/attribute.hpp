// Author: HenryAWE
// License: The 3-clause BSD License


#ifndef TESTWORLD_OBJECT_ATTRIBUTE
#define TESTWORLD_OBJECT_ATTRIBUTE

#include <string>
#include <glm/vec2.hpp>
#include "../graphic/texture.hpp"


namespace awe::attribute
{
    class Base
    {
    public:
        Base() noexcept;
        virtual ~Base() noexcept;

        virtual const char* GetName();
    };

    class Name : public Base
    {
    public:
        Name(const std::string& name_)
            : name(name_) {}

        std::string name;
    };

    class Image2D : public Base
    {
    public:
        Image2D(Texture& tex) : image(&tex) {}

        Texture* image;
    };
}

#endif
