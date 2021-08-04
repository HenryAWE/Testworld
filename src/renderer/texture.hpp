// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RENDERER_TEXTURE_HPP
#define TESTWORLD_RENDERER_TEXTURE_HPP

#include <glad/glad.h>
#include <filesystem>
#include <utility>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>


namespace awe
{
    struct TexDescription
    {
        enum Wrapping : int
        {
            REPEAT,
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER
        };
        enum Filter : int
        {
            LINEAR,
            NEAREST,
        };

        Wrapping s = REPEAT;
        Wrapping t = REPEAT;
        glm::vec4 border_color = glm::vec4(0.0f);
        Filter min = LINEAR;
        Filter mag = LINEAR;
    };

    class Texture
    {
    public:
        typedef GLuint handle;

        Texture() = default;
        Texture(Texture&& move) noexcept
            : m_handle(std::exchange(move.m_handle, 0)) {}
        Texture(const Texture&) = delete;

        ~Texture() noexcept;

        void Generate();
        void Destroy() noexcept;

        bool LoadFile(
            const std::filesystem::path& file,
            bool gen_mipmap = true
        );
        bool LoadFileEx(
            const std::filesystem::path& file,
            bool gen_mipmap,
            TexDescription desc
        );


        [[nodiscard]]
        constexpr handle GetHandle() const noexcept { return m_handle; }
        [[nodiscard]]
        constexpr operator handle() const noexcept { return m_handle; }

        [[nodiscard]]
        constexpr glm::ivec2 GetSize() const noexcept { return m_size; }

    private:
        handle m_handle = 0;
        glm::ivec2 m_size = glm::ivec2(0);
    };
}

#endif