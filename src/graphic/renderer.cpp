// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "renderer.hpp"
#include <cassert>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fmt/format.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include "../window/window.hpp"


namespace awe::graphic
{
    namespace detailed
    {
        FT_Library ftlib = nullptr;
    }

    void Initialize(const AppInitData& initdata)
    {
        FT_Error err;
        err = FT_Init_FreeType(&detailed::ftlib);
        if(err)
        {
            std::string errinfo = fmt::format(
                "FT_Init_FreeType() failed ({}): {}",
                err,
                FT_Error_String(err)
            );
            throw std::runtime_error(errinfo);
        }
    }
    void Deinitialize()
    {
        FT_Done_FreeType(detailed::ftlib);
        detailed::ftlib = nullptr;
    }

    Renderer::Renderer(window::Window& window)
        : m_window(window) {}

    Renderer::~Renderer() noexcept = default;

    bool Renderer::SetVSync(bool enable)
    {
        int result = SDL_GL_SetSwapInterval(enable);
        if(result != 0)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "SDL_GL_SetSwapInterval(1) failed: %s",
                SDL_GetError()
            );
            return false;
        }

        return true;
    }
    bool Renderer::IsVSyncEnabled() noexcept
    {
        return SDL_GL_GetSwapInterval() != 0;
    }

    void Renderer::InitImGuiImpl()
    {
        if(!ImGui_ImplOpenGL3_Init("#version 330 core"))
        {
            throw std::runtime_error("ImGui_ImplOpenGL3_Init() failed");
        }
        if(!ImGui_ImplSDL2_InitForOpenGL(m_window.GetHandle(), SDL_GL_GetCurrentContext()))
        {
            throw std::runtime_error("ImGui_ImplSDL2_InitForOpenGL() failed");
        }
    }
    void Renderer::ShutdownImGuiImpl()
    {
        ImGui_ImplOpenGL3_Shutdown();
    }
    void Renderer::ImGuiImplRenderDrawData()
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
