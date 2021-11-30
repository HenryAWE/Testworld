// Author: HenryAWE
// License: The 3-clause BSD License

#include "buffer.hpp"


namespace awe::graphic
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

    Framebuffer::~Framebuffer() noexcept
    {
        Destroy();
    }

    void Framebuffer::Generate()
    {
        if(m_handle)
            return;
        glGenFramebuffers(1, &m_handle);
    }
    void Framebuffer::Destroy() noexcept
    {
        if(!m_handle)
            return;
        glDeleteFramebuffers(1, &m_handle);
        m_handle = 0;
    }

    Renderbuffer::~Renderbuffer() noexcept
    {
        Destroy();
    }

    void Renderbuffer::Generate()
    {
        if(m_handle)
            return;
        glGenRenderbuffers(1, &m_handle);
    }
    void Renderbuffer::Destroy() noexcept
    {
        if(!m_handle)
            return;
        glDeleteFramebuffers(1, &m_handle);
        m_handle = 0;
    }
}
