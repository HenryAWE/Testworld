// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_RENDERER_HPP
#define TESTWORLD_GRAPHIC_RENDERER_HPP

#include <functional>
#include <future>
#include <mutex>
#include <memory>
#include <string>
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/matrix.hpp>
#include "../sys/init.hpp"
#include "shaderbuilder.hpp"
#include "mesh.hpp"


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
        virtual bool IsInitialized() noexcept;

        // Synchronization
        virtual void BeginMainloop() = 0;
        virtual void QuitMainloop() = 0;
        [[nodiscard]]
        constexpr std::mutex& GetMutex() noexcept { return m_mutex; }
        virtual void Present() = 0;

        virtual std::future<std::string> QueryRendererInfo() = 0;

        std::unique_ptr<Mesh> CreateMesh(bool dynamic = false);

        // Information of renderer

        virtual glm::ivec2 GetDrawableSize() const;
        virtual std::string GetRendererName() = 0;

        // Data
        [[nodiscard]]
        constexpr FT_Library GetFtLib() noexcept { return m_ftlib; }

    protected:
        window::Window& m_window;
        std::mutex m_mutex;

        virtual Mesh* Renderer::NewMesh(bool dynamic) = 0;

        // Call this after derived class is initialized to allocate data of
        // of renderer in correct order
        virtual void NewData();
        // Call this before derived class is deinitialized to release data
        // of renderer in correct order
        virtual void DeleteData() noexcept;

    private:
        bool m_initialized = false;
        FT_Library m_ftlib = nullptr;
    };
}

#endif
