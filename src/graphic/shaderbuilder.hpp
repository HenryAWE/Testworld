// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_SHADERBUILDER_HPP
#define TESTWORLD_GRAPHIC_SHADERBUILDER_HPP

#include <utility>
#include <vector>
#include "shader.hpp"


namespace awe::graphic
{
    class ShaderBuilder
    {
    public:
        bool AddShader(
            GLenum type,
            std::string_view src
        );
        bool AddShaderFromVfs(
            GLenum type,
            const std::string& path
        );

        std::pair<ShaderProgram, bool> Build();

        void Clear() noexcept;

        [[nodiscard]]
        constexpr const std::string& GetLastLog() noexcept { return m_log; }

    private:
        std::vector<Shader> m_shaders;
        std::string m_log;
    };
}


#endif
