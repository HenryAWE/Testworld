// Author: HenryAWE
// License: The 3-clause BSD License

#include "glutil.hpp"


namespace awe::graphic::opengl3
{
    GLint GetInteger(GLenum pname)
    {
        GLint data = 0;
        glGetIntegerv(pname, &data);
        return data;
    }
}
