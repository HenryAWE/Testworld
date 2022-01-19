// Author: HenryAWE
// License: The 3-clause BSD License

#include "texture.hpp"


namespace awe::graphic
{
    TextureDescription::TextureDescription() noexcept
        : wrapping{ TextureWrapping::REPEAT, TextureWrapping::REPEAT },
        filter{ TextureFilter::LINEAR, TextureFilter::LINEAR },
        border_color(glm::u8vec4(0)),
        internal_format(TextureFormat::RGBA),
        mipmap(true) {}

    bool TextureDescription::IsMipmapRequired() const noexcept
    {
        return mipmap;
    }

    TextureFormat GetDefaultFormat(uint8_t channel) noexcept
    {
        switch(channel)
        {
            case 1: return TextureFormat::RED;
            case 3: return TextureFormat::RGB;
            case 4: return TextureFormat::RGBA;
            default: assert(false); return static_cast<TextureFormat>(0);
        }
    }

    ITexture2D::ITexture2D(IRenderer& renderer)
        : Super(renderer) {}

    ITexture2D::~ITexture2D() noexcept = default;

    void ITexture2D::SetTextureDesc(const TextureDescription& desc)
    {
        m_data.desc = desc;
    }

    bool ITexture2D::IsSubmitted() const noexcept
    {
        return m_is_submitted ||
            std::visit([](auto&& arg){ return arg.IsEmpty(); }, m_data.image_data);
    }

    ITexture2D::TextureData& ITexture2D::GetTextureData()
    {
        return m_data;
    }

    void ITexture2D::DataSubmitted()
    {
        m_is_submitted = true;
        ClearData();
    }

    void ITexture2D::ClearData() noexcept
    {
        std::visit([](auto&& arg){ arg.Clear(); }, m_data.image_data);
    }
}
