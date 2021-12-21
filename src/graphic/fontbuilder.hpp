// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_FONTBUILDER_HPP
#define TESTWORLD_GRAPHIC_FONTBUILDER_HPP

#include <glad/glad.h>
#include <array>
#include <map>
#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "opengl3/buffer.hpp"


namespace awe::graphic
{
    namespace detailed
    {
        static constexpr std::size_t MAX_FONT_TEXTURE_UNIT = 1;

        struct CharData
        {
            GLuint texture; // This handle doesn't control the lifetime of the texture
            glm::ivec2 offset; // Offset in texture
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
        };

        struct MeshSection
        {
            opengl3::VertexArray vao;
            opengl3::Buffer vbo;
            std::size_t count = 0; // Character count
            std::array<GLuint, 16> tex; // These handles do not hold the ownerships of the textures
        };
    }

    class TextMesh;

    class FontRenderer
    {
    public:
        FontRenderer();

        ~FontRenderer() noexcept;

        void LoadFontFromMemory(
            std::vector<std::byte> data,
            int index = 0
        );
        void ReleaseFont() noexcept;

        void SetRenderArg(unsigned int height = 48);

        // Build font atlas of characters from [begin, end) region
        void BuildAtlas(char32_t begin, char32_t end);

        [[nodiscard]]
        std::size_t GetFontAtlasCount() noexcept { return m_textures.size();  }
        [[nodiscard]]
        GLuint GetFontAtlas(std::size_t index) const { return m_textures[index]; }

        TextMesh GenerateMesh(glm::vec2 pos, std::u32string_view sv);

    private:
        FT_Face m_face = nullptr;
        std::vector<std::byte> m_memdata; // Used when the font is loaded from memory buffer
        std::vector<GLuint> m_textures;
        boost::container::flat_map<char32_t, detailed::CharData> m_chars;

        bool LoadChar(char32_t cp);

        GLuint AddTexture();
        void ClearTexture() noexcept;
    };

    class TextMesh
    {
        friend class FontRenderer;
    public:
        TextMesh();
        TextMesh(TextMesh&& move) noexcept;
        TextMesh(const TextMesh&) = delete;
        explicit TextMesh(std::vector<detailed::MeshSection> sections);

        // Static method
        // Set the proper blending mode of OpenGL
        static void SetBlendMode();
        // Remark: Call SetBlendMode() first
        void Draw(const glm::mat4& matrix);

    private:
        std::vector<detailed::MeshSection> m_sections;
    };
}


#endif
