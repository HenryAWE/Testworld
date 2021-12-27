// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_RENDERER_HPP
#define TESTWORLD_GRAPHIC_RENDERER_HPP

#include <functional>
#include <mutex>
#include <string>
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/matrix.hpp>
#include "../sys/init.hpp"
#include "shaderbuilder.hpp"


namespace awe::window
{ // Forward declaration
    class Window;
}
namespace awe::graphic
{
    /* OpenGL Renderer */
    class Renderer
    {
    public:
        Renderer(window::Window& window);
        virtual ~Renderer() noexcept;

        virtual bool Initialize();
        virtual void Deinitialize() noexcept;
        virtual bool IsInitialized() noexcept = 0;

        // Synchronization
        virtual void BeginMainloop() = 0;
        virtual void QuitMainloop() = 0;
        [[nodiscard]]
        constexpr std::mutex& GetMutex() noexcept { return m_mutex; }
        virtual void Present() = 0;

        // Information of renderer

        glm::ivec2 GetDrawableSize() const;
        virtual std::string GetRendererName() = 0;

        // Data
        [[nodiscard]]
        constexpr FT_Library GetFtLib() noexcept { return m_ftlib; }

    protected:
        window::Window& m_window;
        std::mutex m_mutex;

    private:
        FT_Library m_ftlib = nullptr;
    };
}

#endif
