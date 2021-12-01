// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_SHADER_HPP
#define TESTWORLD_GRAPHIC_SHADER_HPP

#include <glad/glad.h>
#include <filesystem>
#include <utility>
#include <glm/matrix.hpp>


namespace awe::graphic
{
    class Shader
    {
    public:
        typedef GLuint handle;

        Shader() = default;
        Shader(Shader&& move) noexcept
            : m_handle(std::exchange(move.m_handle, 0)),
            m_type(std::exchange(move.m_type, 0)) {}
        Shader(const Shader&) = delete;

        ~Shader() noexcept;

        void Generate(GLenum type);
        void Destroy() noexcept;

        bool Compile(
            const char* src,
            std::string* log = nullptr
        );

        [[nodiscard]]
        constexpr GLenum GetType() const noexcept { return m_type; }

        [[nodiscard]]
        constexpr handle GetHandle() const noexcept { return m_handle; }
        [[nodiscard]]
        constexpr operator handle() const noexcept { return m_handle; }

    private:
        GLenum m_type = 0;
        handle m_handle = 0;

        void GetLog(std::string* log);
    };

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

        bool Link(
            Shader* shaders,
            size_t count = 1,
            std::string* log = nullptr
        );

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

        void GetLog(std::string* log);
    };

    void Uniform(GLint loc, GLint v);
    void Uniform(GLint loc, GLfloat v);
    void Uniform(GLint loc, const glm::mat4& m);
}

#endif
