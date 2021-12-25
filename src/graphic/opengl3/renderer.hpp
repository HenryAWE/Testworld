// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_RENDERER_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_RENDERER_HPP

#include <glad/glad.h>
#include "glutil.hpp"
#include "../../sys/init.hpp"
#include "../renderer.hpp"


namespace awe::graphic::opengl3
{
    /* OpenGL 3 Renderer */
    class Renderer : public graphic::Renderer
    {
    public:
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

    private:
        void CreateContext(bool debug = false);
        void DestroyContext() noexcept;

        bool m_initialized = false;
        SDL_GLContext m_context = nullptr;
        bool m_debug = false;

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
