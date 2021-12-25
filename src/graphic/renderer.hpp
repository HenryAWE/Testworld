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
#include "opengl3/glutil.hpp"
#include "opengl3/buffer.hpp"
#include "shaderbuilder.hpp"
#include "fontbuilder.hpp"
#include "opengl3/texture.hpp"


namespace awe::window
{ // Forward declaration
    class Window;
}
namespace awe::graphic
{
    using opengl3::Texture;
    using opengl3::Buffer;
    using opengl3::Framebuffer;
    using opengl3::Renderbuffer;

    void Initialize(const AppInitData& initdata);
    void Deinitialize();

    void LoadResource();
    void UnloadResource() noexcept;

    /* OpenGL Renderer */
    class Renderer
    {
    public:
        Renderer(window::Window& window);
        virtual ~Renderer() noexcept;

        virtual bool Initialize() = 0;
        virtual void Deinitialize() noexcept = 0;
        virtual bool IsInitialized() noexcept = 0;


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
        void ImGuiImplRenderDrawData();

        // Information of renderer

        glm::ivec2 GetDrawableSize() const;
        std::string RendererInfo();
        virtual std::string GetRendererName() = 0;

    protected:
        window::Window& m_window;
    };
}

#endif
