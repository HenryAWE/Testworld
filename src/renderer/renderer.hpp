// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RENDERER_RENDERER_HPP
#define TESTWORLD_RENDERER_RENDERER_HPP

#include <glad/glad.h>
#include <string>
#include <SDL.h>
#include "buffer.hpp"
#include "shader.hpp"
#include "texture.hpp"


namespace awe
{
    class Window;

    /* OpenGL Renderer */
    class Renderer
    {
    public:
        Renderer(Window& window);
        ~Renderer() noexcept;

        void CreateContext();
        void DestroyContext() noexcept;

        void Present();

        SDL_GLContext GetContext() const noexcept;

        std::string RendererInfo();

    private:
        Window& m_window;
        SDL_GLContext m_context = nullptr;
    };
}

#endif
