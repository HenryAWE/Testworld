// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_TEXTURE_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_TEXTURE_HPP

#include <glad/glad.h>
#include <filesystem>
#include <utility>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "../texture.hpp"


namespace awe::graphic::opengl3
{
    class Renderer;

    class Texture2D : public ITexture2D
    {
        typedef ITexture2D Super;
    public:
        typedef GLuint handle;

        Texture2D(Renderer& renderer);
        Texture2D(const Texture2D&) = delete;

        ~Texture2D() noexcept;

        [[nodiscard]]
        constexpr handle GetHandle() const noexcept { return m_handle; }
        [[nodiscard]]
        constexpr operator handle() const noexcept { return m_handle; }

        void Submit() override;

        [[nodiscard]]
        glm::ivec2 GetSize() const override;
        [[nodiscard]]
        Renderer& GetRenderer() noexcept;

    private:
        void Initialize();
        void Deinitialize() noexcept;

        handle m_handle = 0;
        glm::ivec2 m_size = glm::ivec2(0);
    };
}

#endif
