// Author: HenryAWE
// License: The 3-clause BSD License

#include "image.hpp"
#include <stb_image.h>
#include <stb_image_write.h>


namespace awe::graphic::common
{
    namespace detailed
    {
        ImageBase::ImageBase() noexcept = default;
        ImageBase::ImageBase(ImageBase&& move) noexcept
            : m_size(std::exchange(move.m_size, glm::ivec2(0))),
            m_raw_data(std::exchange(move.m_raw_data, nullptr)) {}

        ImageBase::~ImageBase() noexcept
        {
            Release();
        }

        void ImageBase::Copy(ImageBase& src, int channel)
        {
            if(!src.m_raw_data)
                return;
            std::size_t bufsize = src.m_size[0] * src.m_size[1] * channel;
            m_raw_data = malloc(bufsize);
            if(!m_raw_data)
                return;
            memcpy(m_raw_data, src.m_raw_data, bufsize);
            m_size = src.m_size;
        }
        void ImageBase::Swap(ImageBase& other) noexcept
        {
            std::swap(m_raw_data, other.m_raw_data);
            std::swap(m_size, other.m_size);
        }

        bool ImageBase::LoadFile(const char* file, int desired_channels, int* channel)
        {
            stbi_set_flip_vertically_on_load(true);
            glm::ivec2 size(0);
            void* tmp = stbi_load(file, &size[0], &size[1], channel, desired_channels);
            if(!tmp)
            {
                return false;
            }

            Release();
            m_raw_data = tmp;
            m_size = size;

            return true;
        }
        bool ImageBase::LoadMemory(
            const void* mem,
            std::size_t length,
            int desired_channels,
            int* channel
        ) {
            glm::ivec2 size(0);
            stbi_uc* tmp = stbi_load_from_memory(
                static_cast<const stbi_uc*>(mem),
                static_cast<int>(length),
                &size[0], &size[1], channel, desired_channels
            );
            if(!tmp)
            {
                return false;
            }

            Release();
            m_raw_data = tmp;
            m_size = size;

            return true;
        }
        bool ImageBase::LoadStream(
            std::istream& is,
            int desired_channels,
            int* channel
        ) {
            if(!is.good())
                return false;

            stbi_io_callbacks cb;
            cb.read = [](void* user, char* data, int size)->int
            {
                return static_cast<int>(
                    static_cast<std::istream*>(user)->readsome(data, size)
                );
            };
            cb.skip = [](void* user, int n)
            {
                static_cast<std::istream*>(user)->seekg(n, std::ios_base::cur);
            };
            cb.eof = [](void* user)->int
            {
                return static_cast<std::istream*>(user)->eof();
            };

            glm::ivec2 size(0);
            stbi_uc* tmp = stbi_load_from_callbacks(
                &cb, &is,
                &size[0], &size[1], channel, desired_channels
            );
            if(!tmp)
            {
                return false;
            }

            Release();
            m_raw_data = tmp;
            m_size = size;

            return true;
        }
        void ImageBase::Release() noexcept
        {
            stbi_image_free(m_raw_data);
            m_raw_data = nullptr;
            m_size = glm::ivec2(0);
        }
    }
}
