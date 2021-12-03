// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "renderer.hpp"
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>
#include <fmt/format.h>
#include <imgui_impl_opengl3.h>
#include "../window/window.hpp"


namespace awe::graphic
{
    Renderer::Renderer(window::Window& window)
        : m_window(window)
    {
        namespace ph = std::placeholders;
        DebugOutputFilter = std::bind(
            &Renderer::DefaultDebugOutputFilter,
            this,
            ph::_1,
            ph::_2,
            ph::_3,
            ph::_4
        );
    }

    Renderer::~Renderer() noexcept
    {
        if(m_context)
            DestroyContext();
    }

    void Renderer::CreateContext(bool debug)
    {
        assert(!m_context);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        if(debug)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        m_context = SDL_GL_CreateContext(m_window.GetHandle());
        if(!m_context)
        {
            throw std::runtime_error(SDL_GetError());
        }
        if(!gladLoadGLLoader(&SDL_GL_GetProcAddress))
        {
            SDL_GL_DeleteContext(m_context);
            m_context = nullptr;
            throw std::runtime_error("gladLoadGLLoader() failed");
        }

        InitData();
    }
    void Renderer::DestroyContext() noexcept
    {
        ReleaseData();
        assert(m_context);
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
    }
    bool Renderer::IsDebugContext()
    {
        int flags = 0;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &flags);
        return flags & SDL_GL_CONTEXT_DEBUG_FLAG;
    }
    void Renderer::AttachDebugCallback()
    {
        // The debug extension is not supported by driver or context
        if(!GLAD_GL_ARB_debug_output || !IsDebugContext())
            return;

        GLDEBUGPROCARB debugproc = [](
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity, 
            GLsizei length,
            const GLchar *message,
            const void *user
        ) {
         try {
                Renderer* renderer = (Renderer*)user;
                renderer->DebugOutput(
                    source,
                    type,
                    id,
                    severity,
                    std::string_view(message, length)
                );
            } catch(...) {}
        };
        glDebugMessageCallbackARB(
            debugproc,
            this
        );
    }
    bool Renderer::DefaultDebugOutputFilter(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity
    ) {
        if(severity == GL_DEBUG_SEVERITY_HIGH_ARB ||
            severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
            return false;

        return true;
    }

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
    void Renderer::ImGuiImplNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
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
            << "  Version: " << glGetString(GL_VERSION) << endl;

        ss << "Context" << endl;
        GLint flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        ss
            << "  Flags: " << fmt::format("0x{0:08X} (0b{0:b})", flags) << endl
            << "  Debug Context: " << (IsDebugContext() ? "true" : "false") << endl;

        ss << "Extensions" << endl;
        ss
            << "  ARB_debug_output = " << GLAD_GL_ARB_debug_output;

        return ss.str();
    }

    Texture Renderer::GeneratePerlinTexture2D(float p, int size)
    {
        auto Fill = [p](int x, int y)->unsigned
        {
            float val = graphic::PerlinNoise<float>(p, (float)x, (float)y);
            return unsigned char(256 * val);
        };
        std::vector<unsigned char> buf;
        buf.resize(size * size);
        for(int i = 0; i < size; ++i)
        {
            for(int j = 0; j < size; ++j)
            {
                buf[i * size + j] = Fill(i, j);
            }
        }
        Texture tex;
        TexDescription desc;
        desc.format = GL_RED;
        desc.type = GL_RED;
        tex.LoadMemoryEx(
            buf.data(),
            glm::ivec2(size),
            true,
            desc
        );
        assert(glGetError() == GL_NO_ERROR);

        return std::move(tex);
    }

    Framebuffer& Renderer::GetFramebuffer()
    {
        return m_fbo;
    }
    Texture& Renderer::GetScreenTexture()
    {
        return m_screen_tex;
    }
    void Renderer::DrawTexture(
        const Texture& tex,
        const glm::mat4& matrix,
        bool custom_shader
    ) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        if(!custom_shader) glUseProgram(m_rect_shader);
        glBindVertexArray(m_rect_vao);

        GLint uniloc = m_rect_shader.UniLoc("matrix");
        if(uniloc != -1) Uniform(uniloc, matrix);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        if(!custom_shader) glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Renderer::InitData()
    {
        InitScreenData();
        InitRectData();

    }
    void Renderer::ReleaseData()
    {
        ReleaseRectData();
        ReleaseScreenData();
    }

    void Renderer::InitScreenData()
    {
        const auto size = GetDrawableSize();

        m_fbo.Generate();
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        m_screen_tex.Generate();
        m_fbo.Generate();
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        TexDescription screnn_desc{};
        screnn_desc.format = GL_RGB;
        m_screen_tex.LoadFramebuffer(
            size,
            screnn_desc,
            GL_COLOR_ATTACHMENT0,
            GL_RGB
        );

        m_rbo.Generate();
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            size[0],
            size[1]
        );
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            m_rbo
        );
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Incomplete OpenGL frame buffer"
            );
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void Renderer::ReleaseScreenData()
    {
        m_fbo.Destroy();
        m_rbo.Destroy();
        m_screen_tex.Destroy();
    }

    void Renderer::InitRectData()
    {
        // rectangle data for drawing texture
        const float vertices[] =
        {
            1.0f, 1.0f,     1.0f, 1.0f,
            1.0f, -1.0f,    1.0f, 0.0f,
            -1.0f, -1.0f,   0.0f, 0.0f,
            -1.0f, 1.0f,    0.0f, 1.0f
        };
        const unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        m_rect_shader.Generate();
        const char* rect_vs =
            "#version 330 core\n"
            "layout (location=0) in vec2 pos;\n"
            "layout (location=1) in vec2 in_coord;\n"
            "out vec2 coord;\n"
            "uniform mat4 matrix;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = matrix * vec4(pos, 0.0f, 1.0f);\n"
                "coord = in_coord;\n"
            "}";
        const char* rect_fs =
            "#version 330 core\n"
            "in vec2 coord;\n"
            "out vec4 frag;\n"
            "uniform sampler2D tex;\n"
            "void main()\n"
            "{\n"
            "   frag = texture(tex, coord);\n"
            "}";
        bool shader_status = m_rect_shader.Compile(rect_vs, rect_fs);
        assert(shader_status);

        m_rect_vao.Generate();
        m_rect_vbo.Generate();
        m_rect_ebo.Generate();

        glBindVertexArray(m_rect_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_rect_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rect_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    void Renderer::ReleaseRectData()
    {
        m_rect_vao.Destroy();
        m_rect_ebo.Destroy();
        m_rect_vbo.Destroy();
        m_rect_shader.Destroy();
    }

    void Renderer::DebugOutput(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity, 
        std::string_view message
    ) {
        if(DebugOutputFilter && DebugOutputFilter(source, type, id, severity))
            return;

        const char* source_str;
        switch(source)
        {
            case GL_DEBUG_SOURCE_API_ARB: source_str = "API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: source_str = "window system"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: source_str = "shader compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: source_str = "third party"; break;
            case GL_DEBUG_SOURCE_APPLICATION_ARB: source_str = "application"; break;
            default:
            case GL_DEBUG_SOURCE_OTHER_ARB: source_str = "other"; break;
        };
        const char* type_str;
        switch(type)
        {
            case GL_DEBUG_TYPE_ERROR_ARB: type_str = "error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: type_str = "deprecated behavior"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: type_str = "undefined behavior"; break;
            case GL_DEBUG_TYPE_PORTABILITY_ARB: type_str = "portablility"; break;
            case GL_DEBUG_TYPE_PERFORMANCE_ARB: type_str = "performance"; break;
            default:
            case GL_DEBUG_TYPE_OTHER_ARB: type_str = "other"; break;
        };
        const char* severity_str;
        switch(severity)
        {
            case GL_DEBUG_SEVERITY_HIGH_ARB: severity_str = "high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM_ARB: severity_str = "medium"; break;
            case GL_DEBUG_SEVERITY_LOW_ARB: severity_str = "low"; break;
            default: severity_str = "other"; break; // Unknown severity
        };

        std::ostream& os = std::cerr;
        os
            << "OpenGL debug output "
            << fmt::format(
                "ID:{} "
                "[{}(0x{:X})]"
                "[{}(0x{:X})]"
                "[{}(0x{:X})]",
                id,
                source_str, source,
                type_str, type,
                severity_str, severity
            )
            << ":\n"
            << message
            << std::endl;
    }
}

// Prefer high-performance graphic cards on Windows
// https://stackoverflow.com/questions/6036292/select-a-graphic-device-in-windows-opengl
#ifdef _WIN32
extern "C" __declspec(dllexport) int32_t NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif
