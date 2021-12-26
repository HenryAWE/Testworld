// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_RENDERER_HPP
#define TESTWORLD_GRAPHIC_RENDERER_HPP

#include <glad/glad.h>
#include <functional>
#include <mutex>
#include <string>
#include <SDL.h>
#include <glm/matrix.hpp>
#include "../sys/init.hpp"
#include "shaderbuilder.hpp"
#include "fontbuilder.hpp"


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

        // Synchronization
        virtual void BeginMainloop() = 0;
        virtual void QuitMainloop() = 0;
        [[nodiscard]]
        constexpr std::mutex& GetMutex() noexcept { return m_mutex; }
        virtual void Present() = 0;

        // ImGui helpers

        void InitImGuiImpl();
        void ShutdownImGuiImpl();
        void ImGuiImplRenderDrawData();

        // Information of renderer

        glm::ivec2 GetDrawableSize() const;
        virtual std::string GetRendererName() = 0;

    protected:
        window::Window& m_window;
        std::mutex m_mutex;
    };
}

#endif
