// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_COMMON_IMAGE_HPP
#define TESTWORLD_GRAPHIC_COMMON_IMAGE_HPP

#include <filesystem>
#include <iosfwd>
#include <utility>
#include <glm/vec2.hpp>


namespace awe::graphic::common
{
    enum class ImageFormat
    {
        BMP = 1,
        PNG
    };

    namespace detailed
    {
        class ImageBase
        {
        public:
            ImageBase() noexcept;
            ImageBase(ImageBase&& move) noexcept;
            ImageBase(const ImageBase&) = delete;

            ~ImageBase() noexcept;

            [[nodiscard]]
            glm::ivec2 Size() const noexcept { return m_size; }
            [[nodiscard]]
            constexpr bool IsEmpty() const noexcept
            {
                return !m_raw_data || m_size[0] == 0 || m_size[1] == 1;
            }

        protected:
            void Copy(const ImageBase& src, int channel);

            void Swap(ImageBase& other) noexcept;

            bool LoadFile(
                const char8_t* file,
                int desired_channels,
                int* channel = nullptr
            );
            bool LoadMemory(
                const void* mem,
                std::size_t length,
                int desired_channels,
                int* channel = nullptr
            );
            bool LoadStream(
                std::istream& is,
                int desired_channels,
                int* channel = nullptr
            );
            bool Allocate(glm::ivec2 size, int channel);
            void Release() noexcept;

            bool WriteFile(
                const char8_t* file,
                int channel,
                ImageFormat format
            );
            bool WriteStream(
                std::ostream& os,
                int channel,
                ImageFormat format
            );

            [[nodiscard]]
            constexpr void* RawData() const noexcept { return m_raw_data; }

        private:
            glm::ivec2 m_size = glm::ivec2(0);
            void* m_raw_data = nullptr;
        };
    }

    template <uint8_t Channel = 4>
    class Image2D : public detailed::ImageBase
    {
        typedef detailed::ImageBase Super;
    public:
        using DataType = std::byte[Channel];
        typedef const DataType ConstDataType;

        Image2D() noexcept = default;
        Image2D(Image2D&& move) noexcept
            : ImageBase(std::move(move)) {}
        Image2D(const Image2D& rhs)
            : ImageBase()
        {
            Super::Copy(rhs, Channel);
        }

        Image2D& operator=(Image2D&& rhs) noexcept
        {
            Image2D(std::move(rhs)).Swap(*this);
            return *this;
        }

        static constexpr uint8_t CHANNEL = Channel;

        bool Load(const std::filesystem::path& file)
        {
            return LoadFile(file.u8string().c_str(), Channel);
        }
        bool Load(const std::vector<std::byte>& data)
        {
            return LoadMemory(data.data(), data.size(), Channel);
        }
        bool Load(std::istream& is)
        {
            return LoadStream(is, Channel);
        }
        bool Copy(const std::byte* bytes, glm::ivec2 size)
        {
            if(!Super::Allocate(size, Channel))
                return false;
            const std::size_t length = size.x * size.y * Channel;
            std::memcpy(RawData(), bytes, length);
            return true;
        }
        bool Allocate(glm::ivec2 size)
        {
            if(!Super::Allocate(size, Channel))
                return false;
            const std::size_t length = size.x * size.y * Channel;
            std::memset(RawData(), 0, length);
            return true;
        }

        bool Write(const std::filesystem::path& file, ImageFormat format = ImageFormat::PNG)
        {
            return WriteFile(file.u8string().c_str(), Channel, format);
        }
        bool Write(std::ostream& os, ImageFormat format = ImageFormat::PNG)
        {
            return WriteStream(os, Channel, format);
        }

        [[nodiscard]]
        DataType& operator[](glm::uvec2 coord)
        {
            return *(DataType*)(Data() + Index(coord));
        }
        [[nodiscard]]
        ConstDataType& operator[](glm::uvec2 coord) const
        {
            return *(ConstDataType*)(Data() + Index(coord));
        }

        [[nodiscard]]
        std::byte* Data() noexcept
        {
            return static_cast<std::byte*>(RawData());
        }
        [[nodiscard]]
        const std::byte* Data() const noexcept
        {
            return static_cast<const std::byte*>(RawData());
        }

        void Swap(Image2D& other) noexcept
        {
            if(this == &other)
                return;
            Super::Swap(other);
        }

        constexpr void Clear() noexcept
        {
            Release();
        }

    private:
        std::size_t Index(glm::uvec2 coord)
        {
            return CHANNEL * (coord[1] * Size()[0] + coord[0]);
        }
    };
}

#endif
