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
#include "../window/window.hpp"


namespace awe::graphic
{
    namespace detailed
    {
        FT_Library ftlib = nullptr;
        std::optional<opengl3::ShaderProgram> text_shader;
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

    void LoadResource()
    {
        ShaderBuilder shbuilder;
        shbuilder.AddShaderFromVfs(GL_VERTEX_SHADER, "shader/opengl3/text.vs");
        shbuilder.AddShaderFromVfs(GL_FRAGMENT_SHADER, "shader/opengl3/text.fs");
        detailed::text_shader.emplace(shbuilder.Build().first);
    }
    void UnloadResource() noexcept
    {
        detailed::text_shader.reset();
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
    void Renderer::Present()
    {
        SDL_GL_SwapWindow(m_window.GetHandle());
    }

    void Renderer::InitImGuiImpl()
    {
        if(!ImGui_ImplOpenGL3_Init("#version 330 core"))
        {
            throw std::runtime_error("ImGui_ImplOpenGL3_Init() failed");
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
    std::string Renderer::RendererInfo()
    {
        using namespace std;
        stringstream ss;

        ss << "OpenGL Information" << endl;
        ss
            << "  Vendor: " << glGetString(GL_VENDOR) << endl
            << "  Renderer: " << glGetString(GL_RENDERER) << endl
            << "  Version: " << glGetString(GL_VERSION) << endl
            << "  Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

        ss << "Driver Support" << endl;
        ss
            << "  Max Texture Size: " << opengl3::GetInteger(GL_MAX_TEXTURE_SIZE) << endl
            << "  Max Texture Image Unit: " << opengl3::GetInteger(GL_MAX_TEXTURE_IMAGE_UNITS) << endl;

        ss << "Context" << endl;
        GLint flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        ss
            << "  Flags: " << fmt::format("0x{0:08X} (0b{0:b})", flags) << endl;

        ss << "Extensions" << endl;
        ss
            << "  ARB_debug_output = " << GLAD_GL_ARB_debug_output;

        return ss.str();
    }
}

// Prefer high-performance graphic cards on Windows
// https://stackoverflow.com/questions/6036292/select-a-graphic-device-in-windows-opengl
#ifdef _WIN32
extern "C" __declspec(dllexport) int32_t NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif
