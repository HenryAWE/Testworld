// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RENDERER_SHADER_HPP
#define TESTWORLD_RENDERER_SHADER_HPP

#include <glad/glad.h>
#include <filesystem>
#include <utility>


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
        bool Load(
            const std::filesystem::path& vspath,
            const std::filesystem::path& fspath
        );

        [[nodiscard]]
        constexpr handle GetHandle() const noexcept { return m_handle; }
        [[nodiscard]]
        constexpr operator handle() const noexcept { return m_handle; }

    private:
        handle m_handle = 0;
    };
}

#endif
