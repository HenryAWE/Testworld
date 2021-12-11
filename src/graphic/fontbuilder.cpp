// Author: HenryAWE
// License: The 3-clause BSD License

#include "fontbuilder.hpp"
#include <cassert>
#include <utility>
#include <SDL.h>


namespace awe::graphic
{
    namespace detailed
    {
        extern FT_Library ftlib;

        void UpdateTexture(glm::uvec2 size, std::byte* data)
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                size[0],
                size[1],
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                data
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }
    }

    class TexBuilder
    {
        std::vector<std::byte> m_data;
        unsigned int m_size;
        unsigned int m_max_height = 0;
        glm::uvec2 m_offset = glm::uvec2(0);
    public:
        TexBuilder(unsigned int size)
            : m_size(size)
        {
            m_data.resize(size * size);
        }

        std::byte& Index(glm::uvec2 idx)
        {
            return m_data[idx[1] * m_size + idx[0]];
        }

        /*
        Return a pair of glm::uvec2 and boolean value.
        If the Boolean value is false, a new bitmap is required.
        If the Boolean value is true, the glm::vec2 will be the offset of the written bitmap in the bigger bitmap
        */
        std::pair<glm::uvec2, bool> Write(unsigned int width, unsigned int height, const std::byte* data)
        {
            glm::uvec2 remained = glm::uvec2(m_size) - m_offset;
            if(remained[1] < height) // No enough space
                return std::pair(m_offset, false);
            if(remained[0] < width)
            {
                m_offset[0] = 0;
                m_offset[1] += m_max_height;
                if(m_offset[1] > m_size || m_size - m_offset[1] < height)
                    return std::pair(m_offset, false); // No enough space
            }
            for(unsigned int i = 0; i < width; ++i)
            {
                for(unsigned int j = 0; j < height; ++j)
                {
                    unsigned int data_idx = j * width + i;
                    Index(glm::uvec2(i, j) + m_offset) = data[data_idx];
                }
            }

            glm::uvec2 offset = m_offset;
            m_offset[0] += width;
            m_max_height = std::max(m_max_height, height);
            return std::pair(m_offset, true);
        }

        [[nodiscard]]
        constexpr unsigned int Size() noexcept { return m_size; }

        [[nodiscard]]
        std::byte* Data() noexcept { return m_data.data(); }
    };

    FontRenderer::FontRenderer() = default;

    FontRenderer::~FontRenderer() noexcept
    {
        ClearTexture();
        ReleaseFont();
    }

    void FontRenderer::LoadFontFromMemory(
        std::vector<std::byte> data,
        int index
    ) {
        FT_Error err;
        m_memdata.swap(data);
        err = FT_New_Memory_Face(
            detailed::ftlib,
            (const FT_Byte*)m_memdata.data(),
            (FT_Long)m_memdata.size(),
            index,
            &m_face
        );
        if(err)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "FT_New_Memory_Face() failed (0x%X): %s",
                err,
                FT_Error_String(err)
            );
        }
    }
    void FontRenderer::ReleaseFont() noexcept
    {
        if(!m_face)
            return;
        FT_Done_Face(m_face);
        m_face = nullptr;
    }

    void FontRenderer::SetRenderArg(unsigned int height)
    {
        FT_Set_Pixel_Sizes(m_face, 0, height);
    }

    void FontRenderer::BuildAtlas(char32_t begin, char32_t end)
    {
        AddTexture();

        TexBuilder builder(1024);
        for(char32_t c = begin; c < end; ++c)
        {
            if(!LoadChar(c))
                continue;
            auto result = builder.Write(
                m_face->glyph->bitmap.width,
                m_face->glyph->bitmap.rows,
                (const std::byte*)m_face->glyph->bitmap.buffer
            );
            if(!result.second)
            { // The space of current builder is not enough
                glBindTexture(GL_TEXTURE_2D, m_textures.back());
                detailed::UpdateTexture(
                    glm::uvec2(builder.Size()),
                    builder.Data()
                );
                glBindTexture(GL_TEXTURE_2D, 0);
                return BuildAtlas(c, end);
            }
            
            CharData chdata;
            chdata.texture = m_textures.back();
            chdata.offset = result.first;
            chdata.size = glm::uvec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows);
            chdata.bearing = glm::uvec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
            chdata.advance = m_face->glyph->advance.x;

            m_chars[c] = chdata;
        }

        glBindTexture(GL_TEXTURE_2D, m_textures.back());
        detailed::UpdateTexture(
            glm::uvec2(builder.Size()),
            builder.Data()
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    bool FontRenderer::LoadChar(char32_t cp)
    {
        FT_Error err = FT_Load_Char(m_face, cp, FT_LOAD_RENDER);
        if(err)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "FT_Load_Char() with code-point 0x%X failed (0x%X): %s",
                cp,
                err,
                FT_Error_String(err)
            );
            return false;
        }
        return true;
    }

    GLuint FontRenderer::AddTexture()
    {
        GLuint tex = 0;
        glGenTextures(1, &tex);
        m_textures.push_back(tex);
        return m_textures.back();
    }
    void FontRenderer::ClearTexture() noexcept
    {
        if(m_textures.empty())
            return;
        glDeleteTextures((GLsizei)m_textures.size(), m_textures.data());
        m_textures.clear();
    }
}
