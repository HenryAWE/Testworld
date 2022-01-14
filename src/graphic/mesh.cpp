// Author: HenryAWE
// License: The 3-clause BSD License

#include "mesh.hpp"
#include <cassert>


namespace awe::graphic
{

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

    Mesh::Mesh(Renderer& renderer, bool dynamic)
        : m_renderer(renderer), m_is_dynamic(dynamic) {}

    void Mesh::AddVertexAttrib(VertexAttribData desc)
    {
        auto& attrs = NewData()->descriptor.attributes;
        attrs.push_back(desc);
    }

    void Mesh::DataSubmitted()
    {
        m_is_submitted = true;
        if(!IsDynamic())
            GetData().reset();
        else
        {
            m_data->vertices.clear();
            m_data->indices.clear();
        }
    }

    std::optional<Mesh::Data>& Mesh::NewData()
    {
        if(!m_data.has_value())
            m_data.emplace();
        return m_data;
    }
}
