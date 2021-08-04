// Author: HenryAWE
// License: The 3-clause BSD License

#include "texture.hpp"
#include <SDL.h>
#include <stb_image.h>


namespace awe
{
    namespace detailed
    {
        GLenum TranslateDesc(TexDescription::Filter filter, bool gen_mipmap) noexcept
        {
            switch(filter)
            {
                default:
                case TexDescription::LINEAR:
                    return gen_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
                case TexDescription::NEAREST:
                    return gen_mipmap ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR;
            }
        }
        GLenum TranslateDesc(TexDescription::Wrapping wrap) noexcept
        {
            switch(wrap)
            {
                default:
                case TexDescription::REPEAT: return GL_REPEAT;
                case TexDescription::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
                case TexDescription::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
                case TexDescription::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            }
        }

        void ApplyDesc(const TexDescription& desc, bool gen_mipmap) noexcept
        {
            auto wrap_s = TranslateDesc(desc.s);
            auto wrap_t = TranslateDesc(desc.t);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TranslateDesc(desc.min, gen_mipmap));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TranslateDesc(desc.mag, false));
            if(wrap_s == GL_CLAMP_TO_BORDER || wrap_t == GL_CLAMP_TO_BORDER)
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &desc.border_color[0]);
        }
    }

    Texture::~Texture() noexcept
    {
        Destroy();
    }

    void Texture::Generate()
    {
        if(m_handle)
            return;
        glGenTextures(1, &m_handle);
    }
    void Texture::Destroy() noexcept
    {
        if(!m_handle)
            return;
        glDeleteTextures(1, &m_handle);
        m_handle = 0;
        m_size = glm::ivec2(0);
    }

    bool Texture::LoadFile(
            const std::filesystem::path& file,
            bool gen_mipmap
    ) {
        TexDescription default_desc;
        default_desc.s = TexDescription::REPEAT;
        default_desc.t = TexDescription::REPEAT;
        default_desc.min = TexDescription::LINEAR;
        default_desc.mag = TexDescription::LINEAR;
        return LoadFileEx(file, gen_mipmap, default_desc);
    }
    bool Texture::LoadFileEx(
            const std::filesystem::path& file,
            bool gen_mipmap,
            TexDescription desc
    ) {
        int width = 0, height = 0;
        stbi_uc* data = stbi_load(
            file.u8string().c_str(),
            &width,
            &height,
            nullptr,
            STBI_rgb_alpha
        );
        if(!data)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Load texture \"%s\" failed: %s",
                file.u8string().c_str(),
                stbi_failure_reason()
            );
            return false;
        }

        if(!m_handle)
            Generate();

        glBindTexture(GL_TEXTURE_2D, m_handle);
        detailed::ApplyDesc(desc, gen_mipmap);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );
        if(gen_mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_size = glm::ivec2(width, height);
        stbi_image_free(data);

        return true;
    }
}