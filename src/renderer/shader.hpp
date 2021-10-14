// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RENDERER_SHADER_HPP
#define TESTWORLD_RENDERER_SHADER_HPP

#include <glad/glad.h>
#include <filesystem>
#include <utility>
#include <glm/matrix.hpp>


namespace awe
{
    class ShaderProgram
    {
    public:
        typedef GLuint handle;

        ShaderProgram() = default;
        ShaderProgram(ShaderProgram&& move) noexcept
            : m_handle(std::exchange(move.m_handle, 0)) {}
        ShaderProgram(const ShaderProgram&) = delete;

        ~ShaderProgram() noexcept;

        void Generate();
        void Destroy() noexcept;

        bool Compile(
            const char* vssrc,
            const char* fssrc
        );
        bool LoadVfs(
            const std::string& vspath,
            const std::string& fspath
        );
        bool Load(
            const std::filesystem::path& vspath,
            const std::filesystem::path& fspath
        );

        [[nodiscard]]
        // uniform location
        GLint UniLoc(const char* name);

        [[nodiscard]]
        constexpr handle GetHandle() const noexcept { return m_handle; }
        [[nodiscard]]
        constexpr operator handle() const noexcept { return m_handle; }

    private:
        handle m_handle = 0;
    };

    void Uniform(GLint loc, const glm::mat4& m);
}

#endif
