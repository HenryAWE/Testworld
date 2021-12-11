// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_FONTBUILDER_HPP
#define TESTWORLD_GRAPHIC_FONTBUILDER_HPP

#include <glad/glad.h>
#include <map>
#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/vec2.hpp>


namespace awe::graphic
{
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

    private:
        struct CharData
        {
            GLuint texture; // This handle doesn't control the lifetime of the texture
            glm::uvec2 offset; // Offset in texture
            glm::uvec2 size;
            glm::uvec2 bearing;
            unsigned int advance;
        };

        FT_Face m_face = nullptr;
        std::vector<std::byte> m_memdata; // Used when the font is loaded from memory buffer
        std::vector<GLuint> m_textures;
        boost::container::flat_map<char32_t, CharData> m_chars;

        bool LoadChar(char32_t cp);

        GLuint AddTexture();
        void ClearTexture() noexcept;
    };
}


#endif
