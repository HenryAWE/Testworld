// Author: HenryAWE
// License: The 3-clause BSD License

#include "shaderbuilder.hpp"
#include "../res/vfs.hpp"


namespace awe::graphic
{
    bool ShaderBuilder::AddShader(
        GLenum type,
        std::string_view src
    ) {
        Shader sh;
        sh.Generate(type);
        if(!sh.Compile(src, &m_log))
            return false;
        m_shaders.emplace_back(std::move(sh));
        return true;
    }
    bool ShaderBuilder::AddShaderFromVfs(
        GLenum type,
        const std::string& path
    ) {
        if(!vfs::Exists(path))
            return false;
        return AddShader(
            type,
            vfs::GetString(path)
        );
    }

    std::pair<ShaderProgram, bool> ShaderBuilder::Build()
    {
        ShaderProgram sh;
        bool result = sh.Link(
            m_shaders.data(),
            m_shaders.size(),
            &m_log
        );
        return std::pair(std::move(sh), result);
    }

    void ShaderBuilder::Clear() noexcept
    {
        m_shaders.clear();
    }
}
