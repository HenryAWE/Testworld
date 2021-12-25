// Author: HenryAWE
// License: The 3-clause BSD License

#include "renderer.hpp"
#include <iostream>
#include <fmt/format.h>
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

        CreateContext(m_debug);
        if(m_debug)
        {
            AttachDebugCallback();
        }

        m_initialized = true;
        return true;
    }
    void Renderer::Deinitialize() noexcept
    {
        if(!IsInitialized())
            return;
        DestroyContext();
        m_initialized = false;
    }
    bool Renderer::IsInitialized() noexcept
    {
        return m_initialized;
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

    std::string Renderer::GetRendererName()
    {
        return "opengl3";
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
