// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_GLUTIL_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_GLUTIL_HPP

#include <glad/glad.h>
#include "../datatype.hpp"


namespace awe::graphic::opengl3
{
    GLenum GetGLType(DataType type);
    GLint GetInteger(GLenum pname);
}


#endif
