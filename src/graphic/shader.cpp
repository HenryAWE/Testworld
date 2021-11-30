// Author: HenryAWE
// License: The 3-clause BSD License

#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <glm/gtc/type_ptr.hpp>
#include "../res/vfs.hpp"


namespace awe::graphic
{
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

    bool ShaderProgram::Compile(
        const char* vssrc,
        const char* fssrc
    ) {
        GLuint vert = 0, frag = 0;
        int status = 0;
        char info[1024] = {};

        vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &vssrc, nullptr);
        glCompileShader(vert);
        glGetShaderiv(vert, GL_COMPILE_STATUS, &status);
        if(!status)
        {
            glGetShaderInfoLog(vert, 1024, nullptr, info);
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Vertex shader error:\n%s",
                info
            );
        }

        frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &fssrc, nullptr);
        glCompileShader(frag);
        glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
        if(!status)
        {
            glGetShaderInfoLog(frag, 1024, nullptr, info);
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Fragment shader error:\n%s",
                info
            );
        }

        if(!m_handle)
            Generate();
        glAttachShader(m_handle, vert);
        glAttachShader(m_handle, frag);
        glLinkProgram(m_handle);
        glGetProgramiv(m_handle, GL_LINK_STATUS, &status);
        if(!status)
        {
            glGetProgramInfoLog(m_handle, 1024, nullptr, info);
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Link error:\n%s",
                info
            );
        }

        glDeleteShader(vert);
        glDeleteShader(frag);

        return status != 0;
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
