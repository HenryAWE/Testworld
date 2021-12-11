// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_RENDERER_HPP
#define TESTWORLD_GRAPHIC_RENDERER_HPP

#include <glad/glad.h>
#include <functional>
#include <string>
#include <SDL.h>
#include <glm/matrix.hpp>
#include "../sys/init.hpp"
#include "glutil.hpp"
#include "buffer.hpp"
#include "shaderbuilder.hpp"
#include "fontbuilder.hpp"
#include "texture.hpp"
#include "noise.hpp"


namespace awe::window
{ // Forward declaration
    class Window;
}
namespace awe::graphic
{
    void Initialize(const AppInitData& initdata);
    void Deinitialize();

    /* OpenGL Renderer */
    class Renderer
    {
    public:
        Renderer(window::Window& window);
        ~Renderer() noexcept;

        // Context management

        void CreateContext(bool debug = false);
        void DestroyContext() noexcept;
        [[nodiscard]]
        constexpr SDL_GLContext GetContext() const noexcept
        {
            return m_context;
        }
        bool IsDebugContext();
        void AttachDebugCallback();
        bool DefaultDebugOutputFilter(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity
        );
        // Return true to ignore the current debug output message
        std::function<bool(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity
        )> DebugOutputFilter;


        // Display controlling

        /*
            Enable or disable the vertical synchronization
            return false if the underlying function fails
        */
        bool SetVSync(bool enable = true);
        bool IsVSyncEnabled() noexcept;
        void Present();

        // ImGui helpers

        void InitImGuiImpl();
        void ShutdownImGuiImpl();
        void ImGuiImplNewFrame();
        void ImGuiImplRenderDrawData();

        // Information of renderer

        glm::ivec2 GetDrawableSize() const;
        std::string RendererInfo();

        // Utilities

        Texture GeneratePerlinTexture2D(float p = 0.5f, int size = 256);

        // Rendering

        Framebuffer& GetFramebuffer();
        Texture& GetScreenTexture();
        void DrawTexture(
            GLuint tex,
            const glm::mat4& matrix = glm::mat4(1),
            bool custom_shader = false
        );
        void DrawTexture(
            const Texture& tex,
            const glm::mat4& matrix = glm::mat4(1),
            bool custom_shader = false
        );

    private:
        window::Window& m_window;
        SDL_GLContext m_context = nullptr;

        void InitData();
        void ReleaseData();

        Framebuffer m_fbo;
        Renderbuffer m_rbo;
        Texture m_screen_tex;
        void InitScreenData();
        void ReleaseScreenData();

        VertexArray m_rect_vao;
        Buffer m_rect_vbo;
        Buffer m_rect_ebo;
        ShaderProgram m_rect_shader;
        void InitRectData();
        void ReleaseRectData();

        void DebugOutput(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity, 
            std::string_view message
        );
    };
}

#endif
