// Author: HenryAWE
// License: The 3-clause BSD License

#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <glm/gtc/type_ptr.hpp>
#include "renderer.hpp"


namespace awe::graphic::opengl3
{
    namespace detailed
    {
        GLenum GetGLShaderType(ShaderType type)
        {
            switch(type)
            {
                case ShaderType::VERTEX: return GL_VERTEX_SHADER;
                case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
                default: assert(false); return GL_INVALID_ENUM;
            }
        }

        std::string ReadStream(std::istream& is)
        {
            std::stringstream ss;
            ss << is.rdbuf();
            return ss.str();
        }
    }

    Shader::~Shader() noexcept
    {
        Destroy();
    }

    void Shader::Generate(GLenum type)
    {
        if(m_handle)
            return;
        m_handle = glCreateShader(type);
        m_type = type;
    }
    void Shader::Destroy() noexcept
    {
        glDeleteShader(m_handle);
        m_handle = 0;
        m_type = 0;
    }

    bool Shader::Compile(
        std::string_view src,
        std::string* log
    ) {
        if(!m_handle)
            return false;

        const GLchar* srcptr = src.data();
        GLint length = static_cast<GLint>(src.length());
        glShaderSource(
            m_handle,
            1,
            &srcptr,
            &length
        );
        glCompileShader(m_handle);
        int status = 0;
        glGetShaderiv(m_handle, GL_COMPILE_STATUS, &status);
        if(!status)
        {
            GetLog(log);
            return false;
        }

        return true;
    }

    void Shader::GetLog(std::string* log)
    {
        if(!log)
            return;
        int length = 0;
        glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &length);
        log->resize(length);
        glGetShaderInfoLog(
            m_handle,
            static_cast<GLsizei>(log->length()),
            nullptr,
            log->data()
        );
    }

    ShaderProgram::ShaderProgram(IRenderer& renderer)
        : Super(renderer) {}

    ShaderProgram::~ShaderProgram()
    {
        Deinitialize();
    }

    void ShaderProgram::Submit()
    {
        std::vector<Shader> shaders;
        for(std::size_t i = 0; i < ShaderSrcCount(); ++i)
        {
            auto& [type, is] = GetSrc(i);
            
            Shader sh;
            sh.Generate(detailed::GetGLShaderType(type));
            sh.Compile(detailed::ReadStream(*is));
            shaders.emplace_back(std::move(sh));
        }
        Link(shaders.data(), shaders.size());

        DataSubmitted();
    }

    GLint ShaderProgram::UniLoc(const char* name)
    {
        return glGetUniformLocation(m_handle, name);
    }

    Renderer& ShaderProgram::GetRenderer() noexcept
    {
        assert(dynamic_cast<Renderer*>(&Super::GetRenderer()));
        return static_cast<Renderer&>(Super::GetRenderer());
    }

    void ShaderProgram::Initialize()
    {
        if(m_handle)
            return;
        m_handle = glCreateProgram();
    }
    void ShaderProgram::Deinitialize() noexcept
    {
        if(!m_handle)
            return;
        GetRenderer().PushClearCommand([handle = m_handle]{
            glDeleteProgram(handle);
        });
        m_handle = 0;
    }

    bool ShaderProgram::Link(
        Shader* shaders,
        size_t count,
        std::string* log
    ) {
        assert(shaders);
        if(!m_handle)
            Initialize();
        for(std::size_t i = 0; i < count; ++i)
            glAttachShader(m_handle, shaders[i]);
        glLinkProgram(m_handle);
        for(std::size_t i = 0; i < count; ++i)
            glDetachShader(m_handle, shaders[i]);

        int status = 0;
        glGetProgramiv(m_handle, GL_LINK_STATUS, &status);
        if(std::string log; !status)
        {
            GetLog(&log);
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Link error:\n%s",
                log.c_str()
            );
            return false;
        }

        return true;
    }

    void ShaderProgram::GetLog(std::string* log)
    {
        if(!log)
            return;
        int length = 0;
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);
        log->resize(length);
        glGetShaderInfoLog(
            m_handle,
            static_cast<GLsizei>(log->length()),
            nullptr,
            log->data()
        );
    }

    void Uniform(GLint loc, GLint v)
    {
        glUniform1i(loc, v);
    }
    void Uniform(GLint loc, GLfloat v)
    {
        glUniform1f(loc, v);
    }
    void Uniform(GLint loc, const glm::mat4& m)
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
    }
}
