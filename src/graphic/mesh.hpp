// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_MESH_HPP
#define TESTWORLD_GRAPHIC_MESH_HPP

#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>
#include <span>
#include "datatype.hpp"
#include "interface.hpp"


namespace awe::graphic
{
    class IRenderer;

    struct VertexAttribData
    {
        int component;
        DataType type;
        bool transpose;

        VertexAttribData(
            int component_,
            DataType type_,
            bool transpose_ = false
        ) noexcept;

        std::size_t Size() const noexcept;
    };

    class VertexDescriptor
    {
    public:
        std::vector<VertexAttribData> attributes;

        std::size_t Size(std::size_t idx) const;
        std::size_t Stride() const;
        std::size_t Offset(std::size_t idx) const;
    };

    class IMesh : public InterfaceBase
    {
        typedef InterfaceBase Super;
    public:
        IMesh(IRenderer& renderer, bool dynamic = false);

        ~IMesh() noexcept;

        // Thread safety: Can only be called in rendering thread
        void Submit();
        // Directly submit data (without caching data first)
        // Thread safety: Can only be called in rendering thread
        void DirectSubmit(
            std::span<const std::byte> vertices,
            const VertexDescriptor& descriptor,
            std::span<const std::byte> indices,
            DataType indices_type
        );
        // Change vertex and index data
        // Thread safety: Can only be called in rendering thread
        void Update(
            std::span<const std::byte> vertices,
            std::span<const std::byte> indices
        );
        // Thread safety: Can only be called in rendering thread
        virtual void Draw() = 0;

        [[nodiscard]]
        constexpr bool IsDynamic() const noexcept { return m_is_dynamic; }
        [[nodiscard]]
        constexpr bool IsSubmitted() const noexcept { return m_is_submitted; }

        template <typename InputIt>
        void AddVertices(InputIt begin, InputIt end)
        {
            using T = std::iterator_traits<InputIt>::value_type;
            std::size_t size = std::distance(begin, end) * sizeof(T);
            auto& vertices = NewData()->vertices;
            vertices.reserve(vertices.size() + size);
            for(auto it = begin; it != end; ++it)
            {
                vertices.resize(vertices.size() + sizeof(T));
                std::memcpy(vertices.data() + vertices.size() - sizeof(T), &*it, sizeof(T));
            }
        }
        template <typename InputIt>
        void AddIndices(InputIt begin, InputIt end)
        {
            using T = std::iterator_traits<InputIt>::value_type;
            std::size_t size = std::distance(begin, end) * sizeof(T);
            auto& indices = NewData()->indices;
            indices.reserve(indices.size() + size);
            for(auto it = begin; it != end; ++it)
            {
                indices.resize(indices.size() + sizeof(T));
                std::memcpy(indices.data() + indices.size() - sizeof(T), &*it, sizeof(T));
            }
        }

        [[nodiscard]]
        DataType GetIndexType() const noexcept;
        template <typename T>
        void SetIndexType()
        {
            NewData()->indices_type = GetDataType<T>();
        }

        void AddVertexAttrib(VertexAttribData desc);

    protected:
        struct Data
        {
            bool submitted = false;
            std::vector<std::byte> vertices;
            VertexDescriptor descriptor;
            std::vector<std::byte> indices;
            DataType indices_type = DataType::UINT;
        };

        // Send data to renderer
        virtual void UpdateData(
            std::span<const std::byte> vertices,
            const VertexDescriptor& descriptor,
            std::span<const std::byte> indices,
            DataType indices_type = DataType::UINT
        ) = 0;
        virtual void UpdateData(
            std::span<const std::byte> vertices,
            std::span<const std::byte> indices
        ) = 0;

        // Call this after successfully sending data to renderer
        void DataSubmitted();

    private:
        [[nodiscard]]
        std::optional<Data>& NewData();

        std::optional<Data> m_data;
        bool m_is_dynamic = false;
        bool m_is_submitted = false;
    };
}


#endif
