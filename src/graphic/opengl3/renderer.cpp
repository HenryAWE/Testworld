// Author: HenryAWE
// License: The 3-clause BSD License

#include "renderer.hpp"
#include <iostream>
#include <sstream>
#include <future>
#include <thread>
#include <fmt/format.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include "../../window/window.hpp"


namespace awe::graphic::opengl3
{
    Renderer::Renderer(
            window::Window& window,
            const AppInitData& initdata
        ) : graphic::Renderer(window),
        m_debug(initdata.ogl_debug) {
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
        Deinitialize();
    }

    bool Renderer::Initialize()
    {
        if(IsInitialized())
            return true;

        bool result = Super::Initialize();
        result = DetachThread();
        m_initialized = result;
        return m_initialized;
    }
    void Renderer::Deinitialize() noexcept
    {
        if(!IsInitialized())
            return;
        m_quit_mainloop = true;
        while(!m_is_data_released)
            std::this_thread::yield();
        Super::Deinitialize();
        m_initialized = false;
    }
    bool Renderer::IsInitialized() noexcept
    {
        return m_initialized;
    }

    void Renderer::BeginMainloop()
    {
        m_begin_mainloop = true;
    }
    void Renderer::QuitMainloop()
    {
        m_quit_mainloop = true;
    }
    void Renderer::Present()
    {
        {
            std::lock_guard lock(GetMutex());
            m_request_render = true;
            m_presented = false;
        }
        while(!m_presented)
            std::this_thread::yield();
        m_presented = false;
    }

    std::unique_ptr<Mesh> Renderer::CreateMesh(bool dynamic)
    {
        return std::unique_ptr<Mesh>(NewMesh(dynamic));
    }

    void Renderer::PushClearCommand(std::function<void()> func)
    {
        std::lock_guard lock(m_clear_cmd_mutex);
        m_clear_cmd.push(std::move(func));
    }

    Mesh* Renderer::NewMesh(bool dynamic)
    {
        return new Mesh(*this, dynamic);
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
    }
    void Renderer::DestroyContext() noexcept
    {
        assert(m_context);
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
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

    std::string Renderer::GetRendererName()
    {
        return "opengl3";
    }

    bool Renderer::DetachThread()
    {
        std::promise<bool> init_result_promise;
        auto init_result = init_result_promise.get_future();
        // Launch render thread
        std::thread([this, result = std::move(init_result_promise)] () mutable {
            m_render_thread_id = std::this_thread::get_id();
            CreateContext(m_debug);
            SDL_LogInfo(
                SDL_LOG_CATEGORY_APPLICATION,
                RendererInfo().c_str()
            );
            if(m_debug)
            {
                AttachDebugCallback();
            }
            InitImGuiImpl();
            result.set_value(true);

            RendererMain();

            ShutdownImGuiImpl();
            DestroyContext();
        }).detach();

        return init_result.get();
    }
    void Renderer::RendererMain()
    {
        while(!m_begin_mainloop)
        {
            if(m_quit_mainloop) return;
            std::this_thread::yield();
        }
        //Begin mainloop in rendering thread
        ImGui_ImplOpenGL3_CreateDeviceObjects();

        while(!m_quit_mainloop)
        {
            // Prepare for rendering next frame
            glClear(GL_COLOR_BUFFER_BIT);

            while(!m_request_render)
            {
                if(m_quit_mainloop) return;
                std::this_thread::yield();
            }
            // Rendering is requested
            {
                std::lock_guard lock(GetMutex());
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                SDL_GL_SwapWindow(m_window.GetHandle());
                m_request_render = false;
                m_presented = true;
            }
            ExecuteClearCommand();

            std::this_thread::yield();
        }
    }
    void Renderer::QuitRenderThread()
    {
        m_quit_mainloop = true;
    }

    void Renderer::AttachDebugCallback()
    {
        int flags = 0;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &flags);
        // The debug extension is not supported by driver or context
        if(!GLAD_GL_ARB_debug_output || !(flags & SDL_GL_CONTEXT_DEBUG_FLAG))
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

    void Renderer::ExecuteClearCommand()
    {
        std::lock_guard lock(m_clear_cmd_mutex);
        while(!m_clear_cmd.empty())
        {
            m_clear_cmd.front()();
            m_clear_cmd.pop();
        }
    }
}
