// Author: HenryAWE
// License: The 3-clause BSD License

#include "texture.hpp"
#include <SDL.h>
#include <stb_image.h>
#include "../../res/vfs.hpp"


namespace awe::graphic::opengl3
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
        void TexImage(
            const common::Image2D<4>& image,
            const TexDescription& desc
        ) {
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                desc.format,
                image.Size()[0],
                image.Size()[1],
                0,
                desc.type,
                GL_UNSIGNED_BYTE,
                image.Data()
            );
        }

        TexDescription DefaultDesc() noexcept
        {
            TexDescription default_desc;
            default_desc.s = TexDescription::REPEAT;
            default_desc.t = TexDescription::REPEAT;
            default_desc.min = TexDescription::LINEAR;
            default_desc.mag = TexDescription::LINEAR;
            return default_desc;
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
        return LoadFileEx(file, gen_mipmap, detailed::DefaultDesc());
    }
    bool Texture::LoadFileEx(
            const std::filesystem::path& file,
            bool gen_mipmap,
            TexDescription desc
    ) {
        common::Image2D image;
        if(!image.Load(file))
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
        detailed::TexImage(image, desc);
        if(gen_mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_size = image.Size();

        return true;
    }
    bool Texture::LoadVfs(
            const std::string& file,
            bool gen_mipmap
    ) {
        return LoadVfsEx(file, gen_mipmap, detailed::DefaultDesc());
    }
    bool Texture::LoadVfsEx(
            const std::string& file,
            bool gen_mipmap,
            TexDescription desc
    ) {
        common::Image2D image;
        if(!image.Load(vfs::GetData(file)))
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Load texture \"%s\" failed: %s",
                file.c_str(),
                stbi_failure_reason()
            );
            return false;
        }

        if(!m_handle)
            Generate();

        glBindTexture(GL_TEXTURE_2D, m_handle);
        detailed::ApplyDesc(desc, gen_mipmap);
        detailed::TexImage(image, desc);
        if(gen_mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_size = image.Size();

        return true;
    }
    bool Texture::LoadFramebuffer(
        glm::ivec2 size,
        TexDescription desc,
        GLenum attachment,
        GLenum format
    ) {
        if(!m_handle)
            Generate();

        glBindTexture(GL_TEXTURE_2D, m_handle);
        detailed::ApplyDesc(desc, false);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            desc.format,
            size[0],
            size[1],
            0,
            format,
            GL_UNSIGNED_BYTE,
            nullptr
        );
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            attachment,
            GL_TEXTURE_2D,
            m_handle,
            0
        );

        m_size = size;

        return true;
    }
}
