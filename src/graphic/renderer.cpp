// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "renderer.hpp"
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <fmt/format.h>
#include "../window/window.hpp"


namespace awe::graphic
{
    Renderer::Renderer(window::Window& window)
        : m_window(window) {}

    Renderer::~Renderer() noexcept = default;

    bool Renderer::Initialize()
    {
        if(m_initialized)
            return;

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
    void Renderer::Deinitialize() noexcept
    {
        if(!m_initialized)
            return;
        FT_Done_FreeType(m_ftlib);
        m_ftlib = nullptr;

        m_initialized = false;
    }
    bool Renderer::IsInitialized() noexcept
    {
        return m_initialized;
    }

    std::unique_ptr<Mesh> Renderer::CreateMesh(bool dynamic)
    {
        return std::unique_ptr<Mesh>(NewMesh(dynamic));
    }

    glm::ivec2 Renderer::GetDrawableSize() const
    {
        return m_window.GetSize();
    }

    void Renderer::NewData() {}
    void Renderer::DeleteData() noexcept {}
}

// Prefer high-performance graphic cards on Windows
// https://stackoverflow.com/questions/6036292/select-a-graphic-device-in-windows-opengl
#ifdef _WIN32
extern "C" __declspec(dllexport) int32_t NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif
