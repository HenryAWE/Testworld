// Author: HenryAWE
// License: The 3-clause BSD License

#include "buffer.hpp"


namespace awe
{
    Buffer::~Buffer() noexcept
    {
        Destroy();
    }

    void Buffer::Generate()
    {
        if(m_handle)
            return;
        glGenBuffers(1, &m_handle);
    }
    void Buffer::Destroy() noexcept
    {
        if(!m_handle)
            return;
        glDeleteBuffers(1, &m_handle);
        m_handle = 0;
    }

    VertexArray::~VertexArray() noexcept
    {
        Destroy();
    }

    void VertexArray::Generate()
    {
        if(m_handle)
            return;
        glGenVertexArrays(1, &m_handle);
    }
    void VertexArray::Destroy() noexcept
    {
        if(!m_handle)
            return;
        glDeleteVertexArrays(1, &m_handle);
        m_handle = 0;
    }
}
