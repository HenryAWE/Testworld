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

        return true;
    }
    void Renderer::Deinitialize() noexcept
    {
        FT_Done_FreeType(m_ftlib);
        m_ftlib = nullptr;
    }

    glm::ivec2 Renderer::GetDrawableSize() const
    {
        glm::ivec2 size;
        SDL_GL_GetDrawableSize(m_window.GetHandle(), &size[0], &size[1]);
        return size;
    }
}

// Prefer high-performance graphic cards on Windows
// https://stackoverflow.com/questions/6036292/select-a-graphic-device-in-windows-opengl
#ifdef _WIN32
extern "C" __declspec(dllexport) int32_t NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif
