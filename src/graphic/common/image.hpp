// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_COMMON_IMAGE_HPP
#define TESTWORLD_GRAPHIC_COMMON_IMAGE_HPP

#include <filesystem>
#include <utility>
#include <glm/vec2.hpp>


namespace awe::graphic::common
{
    namespace detailed
    {
        class ImageBase
        {
        public:
            ~ImageBase() noexcept;

            [[nodiscard]]
            glm::ivec2 Size() const noexcept
            {
                return m_size;
            }

        protected:
            bool LoadFile(
                const char* file,
                int desired_channels,
                int* channel = nullptr
            );
            bool LoadMemory(
                const void* mem,
                std::size_t length,
                int desired_channels,
                int* channel = nullptr
            );
            void Release() noexcept;

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
    public:
        typedef std::byte DataType;
        typedef const DataType ConstDataType;

        static constexpr uint8_t CHANNEL = Channel;

        bool Load(const std::filesystem::path& file)
        {
            return LoadFile(file.u8string().c_str(), Channel);
        }
        bool Load(const std::vector<std::byte>& data)
        {
            return LoadMemory(data.data(), data.size(), Channel);
        }

        DataType& operator[](glm::uvec2 coord)
        {
            const std::size_t idx = CHANNEL * (coord[1] * Size()[0] + coord[0]);
            return Data()[idx];
        }

        [[nodiscard]]
        ConstDataType* Data() const noexcept
        {
            return static_cast<ConstDataType*>(RawData());
        }
    };
}

#endif
