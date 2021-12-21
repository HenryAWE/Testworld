// Author: HenryAWE
// License: The 3-clause BSD License

#include "fontbuilder.hpp"
#include <cassert>
#include <optional>
#include <utility>
#include <boost/container/static_vector.hpp>
#include <SDL.h>
#include "shaderbuilder.hpp"


namespace awe::graphic
{
    namespace detailed
    {
        extern FT_Library ftlib;

        extern std::optional<opengl3::ShaderProgram> text_shader;

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

        class MeshBuilder
        {
            struct CharVertex
            {
                glm::vec2 pos;
                glm::vec2 coord;
                int tex_idx;
            };
            static_assert(std::is_pod_v<CharVertex>);

            std::vector<MeshSection> m_sections;
            // Caches
            std::vector<CharVertex> m_vertices;
            boost::container::static_vector<GLuint, MAX_FONT_TEXTURE_UNIT> m_tex;

            const int m_tex_size = 1024;
        public:
            static constexpr int VERTIECES_PER_CHAR = 6;

            MeshBuilder()
            {
                NewSection();
            }

            // Return: position of next character
            glm::vec2 AddChar(glm::vec2 pos, CharData& chdata)
            {
                if(m_tex.empty())
                    m_tex.push_back(chdata.texture);
                else if(m_tex.back() != chdata.texture)
                {
                    if(m_tex.size() == m_tex.static_capacity)
                    { // New section
                        UpdateSection(m_sections.back());
                        NewSection();
                    }
                    m_tex.push_back(chdata.texture);
                }

                float scale = 1.0f;

                GLfloat xpos = pos[0] + chdata.bearing[0] * scale;
                GLfloat ypos = pos[1] - (chdata.size[1] - chdata.bearing[1]) * scale;

                GLfloat w = chdata.size[0] * scale;
                GLfloat h = chdata.size[1] * scale;

                assert(!m_tex.empty());
                int idx = static_cast<int>(m_tex.size() - 1);

                auto coord = [&chdata, this](glm::ivec2 uv)->glm::vec2
                {
                    uv += chdata.offset;
                    return glm::vec2(uv) / static_cast<float>(m_tex_size);
                };

                std::array<CharVertex, 4> char_vertices
                {
                    // Top left
                    glm::vec2(xpos, ypos + h), coord(glm::ivec2(-chdata.size[0], 0)), idx,
                    // Top right
                    glm::vec2(xpos + w, ypos + h), coord(glm::ivec2(0)), idx,
                    // Bottom right
                    glm::vec2(xpos + w, ypos), coord(glm::ivec2(0, chdata.size[1])), idx,
                    // Bottom left
                    glm::vec2(xpos, ypos), coord(glm::ivec2(-chdata.size[0], chdata.size[1])), idx
                };

                for(std::size_t i : { 0, 3, 2, 0, 2, 1 })
                    m_vertices.push_back(char_vertices[i]);

                m_sections.back().count += 1;

                pos.x += (chdata.advance >> 6) * scale;
                return pos;
            }

            TextMesh BuildMesh()
            {
                if(!m_vertices.empty())
                {
                    UpdateSection(m_sections.back());
                }
                return TextMesh(std::move(m_sections));
            }

        private:
            void NewSection()
            {
                m_vertices.clear();
                m_tex.clear();
                m_sections.emplace_back();
            }

            void UpdateSection(MeshSection& section)
            {
                auto iter = std::copy(m_tex.begin(), m_tex.end(), section.tex.begin());
                std::fill(iter, section.tex.end(), 0);

                section.vao.Generate();
                section.vbo.Generate();
                glBindVertexArray(section.vao);
                glBindBuffer(GL_ARRAY_BUFFER, section.vbo);
                glBufferData(
                    GL_ARRAY_BUFFER,
                    m_vertices.size() * sizeof(CharVertex),
                    m_vertices.data(),
                    GL_DYNAMIC_DRAW
                );
                constexpr GLsizei stride = sizeof(CharVertex);
                glVertexAttribPointer(
                    0,
                    2,
                    GL_FLOAT,
                    GL_FALSE,
                    stride,
                    (void*)0
                );
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(
                    1,
                    2,
                    GL_FLOAT,
                    GL_FALSE,
                    stride,
                    (void*)sizeof(glm::vec2)
                );
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(
                    2,
                    1,
                    GL_INT,
                    GL_FALSE,
                    stride,
                    (void*)(sizeof(glm::vec2) * 2)
                );
                glEnableVertexAttribArray(2);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        };
    }

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

        detailed::TexBuilder builder(1024);
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
            
            detailed::CharData chdata;
            chdata.texture = m_textures.back();
            chdata.offset = result.first;
            chdata.size = glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows);
            chdata.bearing = glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
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

    TextMesh FontRenderer::GenerateMesh(glm::vec2 pos, std::u32string_view sv)
    {
        detailed::MeshBuilder builder;
        for(auto c : sv)
        {
            auto iter = m_chars.find(c);
            if(iter == m_chars.end())
                continue;
            pos = builder.AddChar(pos, iter->second);
        }

        return builder.BuildMesh();
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

    TextMesh::TextMesh() = default;
    TextMesh::TextMesh(TextMesh&& move) noexcept = default;
    TextMesh::TextMesh(std::vector<detailed::MeshSection> sections)
        : m_sections(std::move(sections)) {}

    void TextMesh::Draw(const glm::mat4& matrix)
    {
        glUseProgram(*detailed::text_shader);
        opengl3::Uniform(detailed::text_shader->UniLoc("matrix"), matrix);
        for(auto& section : m_sections)
        {
            std::array<int, 16> tex_arr{0};
            for(int i = 0; i < 16 && section.tex[i] != 0; ++i)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, section.tex[i]);
                tex_arr[i] = i;
            }
            for(int i = 0; i < 16; ++i)
            {
                char tex_i[] = { 't', 'e', 'x', '[', '0' + (char)i, ']', '\0' };
                opengl3::Uniform(detailed::text_shader->UniLoc(tex_i), tex_arr[i]);
            }
            glBindVertexArray(section.vao);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(section.count * 6));
            glBindVertexArray(0);
        }
        glActiveTexture(GL_TEXTURE0);
        glUseProgram(0);
    }
}
