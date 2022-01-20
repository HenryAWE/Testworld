// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_SHADER_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_SHADER_HPP

#include <glad/glad.h>
#include <filesystem>
#include <utility>
#include <glm/matrix.hpp>
#include "../shader.hpp"


namespace awe::graphic::opengl3
{
    class Renderer;

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
            std::string_view src,
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

    class ShaderProgram final : public IShaderProgram
    {
        typedef IShaderProgram Super;
    public:
        typedef GLuint handle;

        ShaderProgram(IRenderer& renderer);
        ShaderProgram(const ShaderProgram&) = delete;

        ~ShaderProgram() noexcept;

        void Submit() override;

        [[nodiscard]]
        // uniform location
        GLint UniLoc(const char* name);

        [[nodiscard]]
        Renderer& GetRenderer() noexcept;

        [[nodiscard]]
        constexpr handle GetHandle() const noexcept { return m_handle; }
        [[nodiscard]]
        constexpr operator handle() const noexcept { return m_handle; }

    private:
        handle m_handle = 0;

        void Initialize();
        void Deinitialize() noexcept;

        bool Link(
            Shader* shaders,
            size_t count = 1,
            std::string* log = nullptr
        );

        void GetLog(std::string* log);
    };

    void Uniform(GLint loc, GLint v);
    void Uniform(GLint loc, GLfloat v);
    void Uniform(GLint loc, const glm::vec3& v);
    void Uniform(GLint loc, const glm::vec4& v);
    void Uniform(GLint loc, const glm::mat4& m);
}

#endif
