// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_RENDERER_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_RENDERER_HPP

#include <glad/glad.h>
#include "../renderer.hpp"
#include <atomic>
#include "../../sys/init.hpp"
#include "glutil.hpp"


namespace awe::graphic::opengl3
{
    /* OpenGL 3 Renderer */
    class Renderer : public graphic::Renderer
    {
    public:
        typedef graphic::Renderer Super;

        Renderer(
            window::Window& window,
            const AppInitData& initdata
        );

        ~Renderer() noexcept;

        virtual bool Initialize() override;
        virtual void Deinitialize() noexcept override;
        virtual bool IsInitialized() noexcept override;

        std::string GetRendererName() override;

        [[nodiscard]]
        constexpr SDL_GLContext GetContext() const noexcept
        {
            return m_context;
        }

        // Return true to ignore the current debug output message
        std::function<bool(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity
        )> DebugOutputFilter;

        void BeginMainloop() override;
        void QuitMainloop() override;

        void Present() override;

    private:
        bool m_initialized = false;

        void CreateContext(bool debug = false);
        void DestroyContext() noexcept;

        // ImGui helpers
        void InitImGuiImpl();
        void ShutdownImGuiImpl();
        void ImGuiImplRenderDrawData();

        std::string RendererInfo();

        SDL_GLContext m_context = nullptr;
        bool m_debug = false;

        // Thread safety: Can only be called in the main thread
        bool DetachThread();
        // Thread safety: Can only be called in the rendering thread
        void RendererMain();
        // Thread safety: Can be called in any thread
        void QuitRenderThread();

        std::thread::id m_render_thread_id;
        std::atomic_bool m_request_render = false;
        std::atomic_bool m_presented = false;
        std::atomic_bool m_quit_mainloop = false;
        std::atomic_bool m_begin_mainloop = false;
        std::atomic_bool m_is_data_released = true;

        void AttachDebugCallback();
        bool DefaultDebugOutputFilter(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity
        );
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
