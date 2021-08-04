// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RENDERER_BUFFER_HPP
#define TESTWORLD_RENDERER_BUFFER_HPP

#include <glad/glad.h>
#include <utility>


namespace awe
{
    class Buffer
    {
    public:
        typedef GLuint handle;

        Buffer() noexcept = default;
        Buffer(Buffer&& move) noexcept
            : m_handle(std::exchange(move.m_handle, 0)) {}
        Buffer(const Buffer&) = delete;

        ~Buffer() noexcept;

        void Generate();
        void Destroy() noexcept;

        [[nodiscard]]
        constexpr handle GetHandle() const noexcept { return m_handle; }
        [[nodiscard]]
        constexpr operator handle() const noexcept { return m_handle; }

    private:
        handle m_handle = 0;
    };

    class VertexArray
    {
    public:
        typedef GLuint handle;

        VertexArray() noexcept = default;
        VertexArray(VertexArray&& move) noexcept
            : m_handle(std::exchange(move.m_handle, 0)) {}
        VertexArray(const Buffer&) = delete;

        ~VertexArray() noexcept;

        void Generate();
        void Destroy() noexcept;

        [[nodiscard]]
        constexpr handle GetHandle() const noexcept { return m_handle; }
        [[nodiscard]]
        constexpr operator handle() const noexcept { return m_handle; }

    private:
        handle m_handle = 0;
    };
}

#endif
