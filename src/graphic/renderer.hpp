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
#include <imgui.h>
#include "../sys/init.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "drawcall.hpp"
#include "common/font.hpp"


namespace awe::window
{ // Forward declaration
    class Window;
}
namespace awe::graphic
{
    class IImGuiRenderer;

    /* Renderer interface */
    class IRenderer
    {
    public:
        IRenderer(window::Window& window);
        virtual ~IRenderer() noexcept;

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

        std::unique_ptr<IMesh> CreateMesh(bool dynamic = false);
        std::unique_ptr<IShaderProgram> CreateShaderProgram();
        std::unique_ptr<ITexture2D> CreateTexture2D();
        std::unique_ptr<IDrawCall> CreateDrawCall();

        // Information of renderer

        virtual glm::ivec2 GetDrawableSize() const;
        virtual std::string GetRendererName() = 0;
        virtual bool IsRuntimeShaderCompilationSupported() const;
        virtual bool IsDataTransposeSupported() const;
        virtual glm::uvec2 MaxTextureSize() const;
        virtual std::size_t MaxTextureUnits() const;

        // Data
        [[nodiscard]]
        constexpr FT_Library GetFtLib() noexcept { return m_ftlib; }
        [[nodiscard]]
        IImGuiRenderer* GetImGuiRenderer() noexcept { return m_imgui.get(); }

    protected:
        window::Window& m_window;
        std::mutex m_mutex;

        virtual IMesh* NewMesh(bool dynamic) = 0;
        virtual IShaderProgram* NewShaderProgram() = 0;
        virtual ITexture2D* NewTexture2D() = 0;
        virtual IDrawCall* NewDrawCall() = 0;

        void SetImGuiRenderer(std::unique_ptr<IImGuiRenderer> renderer);

        // Call this after derived class is initialized to allocate data of
        // of renderer in correct order
        virtual void NewData();
        // Call this before derived class is deinitialized to release data
        // of renderer in correct order
        virtual void DeleteData() noexcept;

    private:
        bool m_initialized = false;
        FT_Library m_ftlib = nullptr;
        std::unique_ptr<IImGuiRenderer> m_imgui;
    };

    class IImGuiRenderer : public InterfaceBase
    {
        typedef InterfaceBase Super;
    public:
        IImGuiRenderer(IRenderer& renderer);

        ~IImGuiRenderer() noexcept;

        // Thread safety: Can only be called in rendering thread
        virtual void CreateDeviceObjects() = 0;
        // Thread safety: Can only be called in rendering thread
        virtual void DestroyDeviceObjects() noexcept = 0;

        // Thread safety: Can only be called in rendering thread
        virtual void Draw(ImDrawData* drawdata) = 0;

        [[nodiscard]]
        virtual const std::string& GetName() = 0;

        [[nodiscard]]
        virtual ImTextureID ToImTextureId(const ITexture2D& tex) noexcept = 0;
    };
}

#endif
