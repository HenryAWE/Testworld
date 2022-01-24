// Author: HenryAWE
// License: The 3-clause BSD License

#include "mesh.hpp"
#include <cassert>


namespace awe::graphic
{
    VertexAttribData::VertexAttribData(
        int component_,
        DataType type_,
        bool transpose_
    ) noexcept : component(component_),
        type(type_),
        transpose(transpose_) {}

    std::size_t VertexAttribData::Size() const noexcept
    {
        return SizeOf(type) * component;
    }

    std::size_t VertexDescriptor::Size(std::size_t idx) const
    {
        return attributes[idx].Size();
    }
    std::size_t VertexDescriptor::Stride() const
    {
        std::size_t stride = 0;
        for(size_t i = 0; i < attributes.size(); ++i)
            stride += Size(i);
        return stride;
    }
    std::size_t VertexDescriptor::Offset(std::size_t idx) const
    {
        std::size_t offset = 0;
        if(idx != 0)
            offset += Offset(idx - 1) + Size(idx - 1);
        return offset;
    }

    IMesh::IMesh(IRenderer& renderer, bool dynamic)
        : Super(renderer), m_is_dynamic(dynamic) {}

    IMesh::~IMesh() noexcept = default;

    DataType IMesh::GetIndexType() const noexcept
    {
        return m_data ? m_data->indices_type : static_cast<DataType>(0);
    }

    void IMesh::AddVertexAttrib(VertexAttribData desc)
    {
        auto& attrs = NewData()->descriptor.attributes;
        attrs.push_back(desc);
    }

    void IMesh::DirectSubmit(
        std::span<const std::byte> vertices,
        const VertexDescriptor& descriptor,
        std::span<const std::byte> indices,
        DataType indices_type
    ) {
        UpdateData(
            vertices,
            descriptor,
            indices,
            indices_type
        );
        DataSubmitted();
    }
    void IMesh::Update(
        std::span<const std::byte> vertices,
        std::span<const std::byte> indices
    ) {
        UpdateData(vertices, indices);
    }
    void IMesh::Submit()
    {
        if(IsSubmitted() || !m_data)
            return;

        auto& data = *m_data;
        UpdateData(
            data.vertices,
            data.descriptor,
            data.indices,
            data.indices_type
        );
        DataSubmitted();
    }

    void IMesh::DataSubmitted()
    {
        m_is_submitted = true;
        if(!IsDynamic())
            m_data.reset();
        else
        { // Do not free memory
            m_data->vertices.clear();
            m_data->indices.clear();
        }
    }

    std::optional<IMesh::Data>& IMesh::NewData()
    {
        if(!m_data.has_value())
            m_data.emplace();
        return m_data;
    }
}
