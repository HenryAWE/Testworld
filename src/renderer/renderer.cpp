// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "renderer.hpp"
#include <cassert>
#include <stdexcept>
#include "../window/window.hpp"


namespace awe
{
    Renderer::Renderer(Window& window)
        : m_window(window) {}

    Renderer::~Renderer() noexcept
    {
        if(m_context)
            DestroyContext();
    }

    void Renderer::CreateContext()
    {
        assert(!m_context);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        m_context = SDL_GL_CreateContext(m_window.GetHandle());
        if(!m_context)
        {
            throw std::runtime_error(SDL_GetError());
        }
        if(!gladLoadGLLoader(&SDL_GL_GetProcAddress))
        {
            SDL_GL_DeleteContext(m_context);
            m_context = nullptr;
            throw std::runtime_error("gladLoadGLLoader() failed");
        }
    }
    void Renderer::DestroyContext() noexcept
    {
        assert(m_context);
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
    }

    void Renderer::Present()
    {
        SDL_GL_SwapWindow(m_window.GetHandle());
    }
}
