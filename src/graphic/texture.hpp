// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_TEXTURE_HPP
#define TESTWORLD_GRAPHIC_TEXTURE_HPP

#include <array>
#include <variant>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "interface.hpp"
#include "datatype.hpp"
#include "common/image.hpp"


namespace awe::graphic
{
    enum class TextureFormat : int
    {
        RED = 1,
        RGB,
        RGBA
    };
    enum class TextureWrapping : int
    {
        REPEAT = 1,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };
    enum class TextureFilter : int
    {
        LINEAR = 1,
        NEAREST
    };
    class TextureDescription
    {
    public:
        std::array<TextureWrapping, 2> wrapping; // Horizontal and vertical
        std::array<TextureFilter, 2> filter; // Minification and magnification
        glm::u8vec4 border_color; // RGBA 8888
        TextureFormat internal_format;
        bool mipmap;

        TextureDescription() noexcept;
        TextureDescription(const TextureDescription&) = default;

        TextureDescription& operator=(const TextureDescription&) = default;

        bool IsMipmapRequired() const noexcept;
    };

    TextureFormat GetDefaultFormat(uint8_t channel) noexcept;

    class ITexture2D : public InterfaceBase
    {
        typedef InterfaceBase Super;
    public:
        ITexture2D(IRenderer& renderer);

        ~ITexture2D() noexcept;

        template <std::uint8_t Channel>
        void LoadImage(common::Image2D<Channel> image)
        {
            m_data.image_data = std::move(image);
        }

        void SetTextureDesc(const TextureDescription& desc);

        virtual void Submit() = 0;

        virtual glm::ivec2 GetSize() const = 0;

        [[nodiscard]]
        bool IsSubmitted() const noexcept;

    protected:
        typedef std::variant<
            common::Image2D<1>,
            common::Image2D<3>,
            common::Image2D<4>
        > TextureImageData;

        struct TextureData
        {
            TextureImageData image_data;
            TextureDescription desc;
        };

        TextureData& GetTextureData();

        void DataSubmitted();

    private:
        TextureData m_data;
        bool m_is_submitted = false;

        void ClearData() noexcept;
    };
}


#endif
