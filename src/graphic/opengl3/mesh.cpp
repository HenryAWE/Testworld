// Author: HenryAWE
// License: The 3-clause BSD License

#include "mesh.hpp"
#include <cassert>
#include "renderer.hpp"


namespace awe::graphic::opengl3
{
    namespace detailed
    {
        void ApplyVertexAttrib(
            GLuint idx,
            const VertexAttribData& attr,
            GLsizei stride,
            std::size_t offset
        ) {
            glVertexAttribPointer(
                idx,
                attr.component,
                GetGLType(attr.type),
                GL_FALSE,
                stride,
                reinterpret_cast<const void*>(offset)
            );
            glEnableVertexAttribArray(idx);
        }
    }

    Mesh::Mesh(Renderer& renderer, bool dynamic)
        : Super(renderer, dynamic) {}
    Mesh::~Mesh() noexcept
    {
        Deinitialize();
    }

    void Mesh::Draw()
    {
        Submit();
        glBindVertexArray(m_gldata.vao);
        glDrawElements(
            m_drawcfg.mode,
            m_drawcfg.count,
            m_drawcfg.type,
            reinterpret_cast<const void*>(0)
        );
        glBindVertexArray(0);
    }

    Renderer& Mesh::GetRenderer() noexcept
    {
        assert(dynamic_cast<Renderer*>(&Super::GetRenderer()));
        return static_cast<Renderer&>(Super::GetRenderer());
    }

    void Mesh::Initialize()
    {
        if(m_init)
            return;

        glGenVertexArrays(1, &m_gldata.vao);
        GLuint buffers[2];
        glGenBuffers(2, buffers);
        m_gldata.vbo = buffers[0];
        m_gldata.ebo = buffers[1];

        m_init = true;
    }
    void Mesh::Deinitialize() noexcept
    {
        if(!m_init)
            return;

        GetRenderer().PushClearCommand([gldata = m_gldata]{
            glDeleteVertexArrays(1, &gldata.vao);
            const GLuint buffers[2] { gldata.vbo, gldata.ebo };
            glDeleteBuffers(2, buffers);
        });
        std::memset(&m_gldata, 0, sizeof(GLData));
        m_init = false;
    }

    void Mesh::UpdateData(
            std::span<std::byte> vertices,
            const VertexDescriptor& descriptor,
            std::span<std::byte> indices,
            DataType indices_type
    ) {
        if(!m_init)
            Initialize();
        glBindVertexArray(m_gldata.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_gldata.vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size(),
            vertices.data(),
            IsDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_READ
        );
        const std::size_t stride = descriptor.Stride();
        for(std::size_t i = 0; i < descriptor.attributes.size(); ++i)
        {
            detailed::ApplyVertexAttrib(
                static_cast<GLuint>(i),
                descriptor.attributes[i],
                static_cast<GLsizei>(stride),
                descriptor.Offset(i)
            );
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gldata.ebo);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size(),
            indices.data(),
            IsDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
        );
        glBindVertexArray(0);

        m_drawcfg.mode = GL_TRIANGLES;
        m_drawcfg.count = static_cast<GLsizei>(indices.size() / SizeOf(indices_type));
        m_drawcfg.type = GetGLType(indices_type);
    }
}
