// Author: HenryAWE
// License: The 3-clause BSD License

#include "glutil.hpp"
#include <cassert>


namespace awe::graphic::opengl3
{
    GLenum GetGLType(DataType type)
    {
        switch(type)
        {
        case DataType::FLOAT: return GL_FLOAT;
        case DataType::BYTE: return GL_BYTE;
        case DataType::UBYTE: return GL_UNSIGNED_BYTE;
        case DataType::SHORT: return GL_SHORT;
        case DataType::USHORT: return GL_UNSIGNED_SHORT;
        case DataType::INT: return GL_INT;
        case DataType::UINT: return GL_UNSIGNED_INT;
        default: assert(false); return GL_INVALID_ENUM;
        }
    }

    GLint GetInteger(GLenum pname)
    {
        GLint data = 0;
        glGetIntegerv(pname, &data);
        return data;
    }
}
