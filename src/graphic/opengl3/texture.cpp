// Author: HenryAWE
// License: The 3-clause BSD License

#include "texture.hpp"
#include <SDL.h>
#include <stb_image.h>
#include "renderer.hpp"


namespace awe::graphic::opengl3
{
    namespace detailed
    {
        GLenum TranslateFilter(TextureFilter filter, bool gen_mipmap) noexcept
        {
            switch(filter)
            {
                default:
                case TextureFilter::LINEAR:
                    return gen_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
                case TextureFilter::NEAREST:
                    return gen_mipmap ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR;
            }
        }
        GLenum TranslateWrapping(TextureWrapping wrap) noexcept
        {
            switch(wrap)
            {
                case TextureWrapping::REPEAT: return GL_REPEAT;
                case TextureWrapping::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
                case TextureWrapping::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
                case TextureWrapping::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
                default: assert(false); return GL_INVALID_ENUM;
            }
        }
        GLenum TranslateFormat(TextureFormat format) noexcept
        {
            switch(format)
            {
                case TextureFormat::RED: return GL_RED;
                case TextureFormat::RGB: return GL_RGB;
                case TextureFormat::RGBA: return GL_RGBA;
                default: assert(false); return GL_INVALID_ENUM;
            }
        }

        void ApplyDesc(const TextureDescription& desc) noexcept
        {
            bool gen_mipmap = desc.IsMipmapRequired();
            auto wrap_s = TranslateWrapping(desc.wrapping[0]);
            auto wrap_t = TranslateWrapping(desc.wrapping[1]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TranslateFilter(desc.filter[0], gen_mipmap));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TranslateFilter(desc.filter[1], false)); // Never use mipmap filter on magnification
            if(wrap_s == GL_CLAMP_TO_BORDER || wrap_t == GL_CLAMP_TO_BORDER)
            {
                glm::vec4 color;
                for(auto i : { 0, 1, 2, 3 })
                    color[i] = static_cast<float>(desc.border_color[i]) / 256.0f;
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
            }
        }
        template <uint8_t Channel>
        void TexImage(
            const common::Image2D<Channel>& image,
            const TextureDescription& desc
        ) {
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                TranslateFormat(desc.internal_format),
                image.Size()[0],
                image.Size()[1],
                0,
                TranslateFormat(GetDefaultFormat(image.CHANNEL)),
                GL_UNSIGNED_BYTE,
                image.Data()
            );
        }
    }

    Texture2D::Texture2D(Renderer& renderer)
        : Super(renderer) {}

    Texture2D::~Texture2D() noexcept
    {
        Deinitialize();
    }

    void Texture2D::Submit()
    {
        if(!m_handle)
            Initialize();

        auto& data = GetTextureData();
        glBindTexture(GL_TEXTURE_2D, m_handle);
        detailed::ApplyDesc(data.desc);
        std::visit(
            [&data](auto&& arg){ detailed::TexImage(arg, data.desc); },
            data.image_data
        );
        if(data.desc.IsMipmapRequired())
            glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        DataSubmitted();
    }

    glm::ivec2 Texture2D::GetSize() const
    {
        return m_size;
    }
    Renderer& Texture2D::GetRenderer() noexcept
    {
        assert(dynamic_cast<Renderer*>(&Super::GetRenderer()));
        return static_cast<Renderer&>(Super::GetRenderer());
    }

    void Texture2D::Initialize()
    {
        if(m_handle)
            return;
        glGenTextures(1, &m_handle);
    }
    void Texture2D::Deinitialize() noexcept
    {
        if(!m_handle)
            return;
        GetRenderer().PushClearCommand([handle = m_handle]{
            glDeleteTextures(1, &handle);
        });
        m_handle = 0;
    }
}
