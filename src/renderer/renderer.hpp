// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RENDERER_RENDERER_HPP
#define TESTWORLD_RENDERER_RENDERER_HPP

#include <glad/glad.h>
#include <string>
#include <SDL.h>
#include <glm/matrix.hpp>
#include "buffer.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "noise.hpp"


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

        glm::ivec2 GetDrawableSize() const;
        std::string RendererInfo();

        // Utilities

        Texture GeneratePerlinTexture2D(float p = 0.5f, int size = 256);

        // Rendering

        Framebuffer& GetFramebuffer();
        Texture& GetScreenTexture();
        void DrawTexture(
            const Texture& tex,
            const glm::mat4& matrix = glm::mat4(1),
            bool custom_shader = false
        );

    private:
        Window& m_window;
        SDL_GLContext m_context = nullptr;

        void InitData();
        void ReleaseData();

        Framebuffer m_fbo;
        Renderbuffer m_rbo;
        Texture m_screen_tex;

        VertexArray m_rect_vao;
        Buffer m_rect_vbo;
        Buffer m_rect_ebo;
        ShaderProgram m_rect_shader;
    };
}

#endif
