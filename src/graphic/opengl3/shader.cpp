// Author: HenryAWE
// License: The 3-clause BSD License

#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <glm/gtc/type_ptr.hpp>
#include "../../res/vfs.hpp"


namespace awe::graphic::opengl3
{
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

    ShaderProgram::~ShaderProgram()
    {
        Destroy();
    }

    void ShaderProgram::Generate()
    {
        if(m_handle)
            return;
        m_handle = glCreateProgram();
    }
    void ShaderProgram::Destroy() noexcept
    {
        if(!m_handle)
            return;
        glDeleteProgram(m_handle);
        m_handle = 0;
    }

    bool ShaderProgram::Link(
        Shader* shaders,
        size_t count,
        std::string* log
    ) {
        assert(shaders);
        if(!m_handle)
            Generate();
        for(size_t i = 0; i < count; ++i)
            glAttachShader(m_handle, shaders[i]);
        glLinkProgram(m_handle);
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

    bool ShaderProgram::Compile(
        std::string_view vssrc,
        std::string_view fssrc
    ) {
        Shader shaders[2]; // Vertex and fragment shaders
        shaders[0].Generate(GL_VERTEX_SHADER);
        if(std::string log; !shaders[0].Compile(vssrc, &log))
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Vertex shader error:\n%s",
                log.c_str()
            );
            return false;
        }
        shaders[1].Generate(GL_FRAGMENT_SHADER);
        if(std::string log; !shaders[1].Compile(fssrc, &log))
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Fragment shader error:\n%s",
                log.c_str()
            );
            return false;
        }

        if(!m_handle)
            Generate();
        return Link(shaders, 2);
    }
    bool ShaderProgram::LoadVfs(
        const std::string& vspath,
        const std::string& fspath
    ) {
        return Compile(
            vfs::GetString(vspath).c_str(),
            vfs::GetString(fspath).c_str()
        );
    }
    bool ShaderProgram::Load(
        const std::filesystem::path& vspath,
        const std::filesystem::path& fspath
    ) {
        using namespace std;

        auto read = [](const filesystem::path& srcpath)
        {
            ifstream ifs(srcpath);
            if(!ifs)
                return string();

            stringstream ss;
            ss << ifs.rdbuf();

            return ss.str();
        };

        return Compile(
            read(vspath).c_str(),
            read(fspath).c_str()
        );
    }

    GLint ShaderProgram::UniLoc(const char* name)
    {
        return glGetUniformLocation(m_handle, name);
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