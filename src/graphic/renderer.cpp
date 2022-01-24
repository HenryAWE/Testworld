// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "renderer.hpp"
#include <cassert>
#include <ranges>
#include <stdexcept>
#include <sstream>
#include <fmt/format.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../window/window.hpp"


namespace awe::graphic
{
    IRenderer::IRenderer(window::Window& window)
        : m_window(window) {}

    IRenderer::~IRenderer() noexcept = default;

    bool IRenderer::Initialize()
    {
        if(m_initialized)
            return false;

        FT_Error err;
        err = FT_Init_FreeType(&m_ftlib);
        if(err)
        {
            std::string errinfo = fmt::format(
                "FT_Init_FreeType() failed ({}): {}",
                err,
                FT_Error_String(err)
            );
            return false;
        }

        m_initialized = true;
        return m_initialized;
    }
    void IRenderer::Deinitialize() noexcept
    {
        if(!m_initialized)
            return;
        FT_Done_FreeType(m_ftlib);
        m_ftlib = nullptr;

        m_initialized = false;
    }
    bool IRenderer::IsInitialized() noexcept
    {
        return m_initialized;
    }

    std::unique_ptr<IMesh> IRenderer::CreateMesh(bool dynamic)
    {
        return std::unique_ptr<IMesh>(NewMesh(dynamic));
    }
    std::unique_ptr<IShaderProgram> IRenderer::CreateShaderProgram()
    {
        return std::unique_ptr<IShaderProgram>(NewShaderProgram());
    }
    std::unique_ptr<ITexture2D> IRenderer::CreateTexture2D()
    {
        return std::unique_ptr<ITexture2D>(NewTexture2D());
    }
    std::unique_ptr<IDrawCall> IRenderer::CreateDrawCall()
    {
        return std::unique_ptr<IDrawCall>(NewDrawCall());
    }

    glm::ivec2 IRenderer::GetDrawableSize() const
    {
        return m_window.GetSize();
    }

    bool IRenderer::IsRuntimeShaderCompilationSupported() const
    {
        return false;
    }
    bool IRenderer::IsDataTransposeSupported() const
    {
        return false;
    }
    std::size_t IRenderer::MaxTextureUnit() const
    {
        return 16;
    }

    void IRenderer::SetImGuiRenderer(std::unique_ptr<IImGuiRenderer> renderer)
    {
        m_imgui = std::move(renderer);
    }

    void IRenderer::NewData() {}
    void IRenderer::DeleteData() noexcept {}

    namespace detailed
    {
        template <typename T>
        std::span<const std::byte> GetBytes(const ImVector<T>& vec)
        {
            return std::span<const std::byte>(
                (const std::byte*)vec.Data,
                vec.size_in_bytes()
            );
        }
    }

    IImGuiRenderer::IImGuiRenderer(IRenderer& renderer)
        : Super(renderer) {}

    IImGuiRenderer::~IImGuiRenderer() noexcept = default;
}

// Prefer high-performance graphic cards on Windows
// https://stackoverflow.com/questions/6036292/select-a-graphic-device-in-windows-opengl
#ifdef _WIN32
extern "C" __declspec(dllexport) int32_t NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif
