// Author: HenryAWE
// License: The 3-clause BSD License

#include "font.hpp"
#include <SDL.h>
#include "../../res/vfs.hpp"


namespace awe::graphic::common
{
    namespace detailed
    {
        bool LoadGlyph(FT_Face face, char32_t cp, bool color = false)
        {
            FT_Int32 load_flags = FT_LOAD_RENDER;
            if(color)
                load_flags |= FT_LOAD_COLOR;

            FT_UInt index = FT_Get_Char_Index(face, cp);
            if(index == 0)
                return false;
            FT_Error err = FT_Load_Glyph(face, index, load_flags);

            return err == FT_Err_Ok;
        }

        glm::uvec2 CopyGlyphBitmap(
            Image2D<1>& img,
            const FT_Bitmap& bm,
            glm::uvec2 offset
        ) {
            for(unsigned int i = 0; i < bm.width; ++i)
            {
                for(unsigned int j = bm.rows; j > 0; --j)
                {
                    const int bm_idx = (j - 1) * bm.width + i;
                    img[offset + glm::uvec2(i, bm.rows - j)][0] = (std::byte)bm.buffer[bm_idx];
                }
            }

            return offset + glm::uvec2(bm.width, 0);
        }

        GlyphLookupTable::GlyphData GenerateGlyphData(
            FT_GlyphSlot glyph,
            glm::uvec2 offset
        ) {
            GlyphLookupTable::GlyphData data;
            data.coord = offset;
            data.size = glm::uvec2(glyph->bitmap.width, glyph->bitmap.rows);
            data.bearing = glm::ivec2(glyph->bitmap_left, glyph->bitmap_top);
            data.advance = glyph->advance.x;

            return data;
        }
    }

    FontBuilder::FontBuilder(FT_Library ftlib)
        : m_ftlib(ftlib), m_face(nullptr) {}

    FontBuilder::~FontBuilder() noexcept
    {
        if(m_face)
            FT_Done_Face(m_face);
    }

    bool FontBuilder::LoadFontVfs(
        const std::string& file,
        glm::uvec2 size
    ) {
        if(!vfs::Exists(file))
            return false;
        auto tmp = vfs::GetData(file);
        FT_Face face = nullptr;
        FT_Error err = FT_New_Memory_Face(
            GetLibrary(),
            (const FT_Byte*)tmp.data(),
            (FT_Long)tmp.size(),
            0,
            &face
        );
        if(err != FT_Err_Ok)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "FT_New_Memory_Face() from VFS file \"%s\" failed: %s",
                file.c_str(),
                FT_Error_String(err)
            );
            return false;
        }
        Release();
        m_face = face;
        m_buffer.swap(tmp);

        FT_Set_Pixel_Sizes(m_face, size[0], size[1]);

        return true;
    }

    void FontBuilder::Release() noexcept
    {
        if(!m_face)
            return;
        FT_Done_Face(m_face);
        m_face = nullptr;
    }

    std::pair<Image2D<1>, GlyphLookupTable> FontBuilder::GenerateAtlas(char32_t begin, char32_t end)
    {
        common::Image2D<1> atlas;
        GlyphLookupTable table;
        atlas.Allocate(glm::ivec2(1024));
        glm::ivec2 offset = glm::ivec2(0);

        unsigned int max_height = 0;
        for(char32_t c = begin; c < end; ++c)
        {
            if(!detailed::LoadGlyph(m_face, c))
                continue;
            glm::ivec2 remained = atlas.Size() - offset;
            if(remained[0] < (int)m_face->glyph->bitmap.width)
            {
                if(remained[1] < (int)(m_face->glyph->bitmap.rows + max_height))
                    break;
                offset[0] = 0;
                offset[1] += max_height;
                max_height = 0;
            }
            auto& container = table.GetContainer();
            container.insert(std::pair(c, detailed::GenerateGlyphData(m_face->glyph, offset)));
            offset = detailed::CopyGlyphBitmap(
                atlas,
                m_face->glyph->bitmap,
                offset
            );

            max_height = std::max(max_height, m_face->glyph->bitmap.rows);
            if(offset[0] > atlas.Size()[0])
            {
                offset[0] = 0;
                offset[1] += max_height;
                max_height = 0;
                if(offset[1] > atlas.Size()[1])
                    break;
            }
        }

        return std::pair(std::move(atlas), std::move(table));
    }
}
