// Author: HenryAWE
// License: The 3-clause BSD License

#include "drawcall.hpp"
#include <cassert>
#include "renderer.hpp"


namespace awe::graphic
{
    DrawCallSettings::DrawCallSettings() noexcept
        : depth_test(false),
        mesh(false),
        blending(true),
        culling(Culling::Back) {}

    IDrawCall::IDrawCall(IRenderer& renderer)
        : Super(renderer)
    {
        m_textures.reserve(renderer.MaxTextureUnits());
    }

    void IDrawCall::Submit()
    {
        if(!m_mesh->IsSubmitted())
            m_mesh->Submit();
        if(!m_program->IsSubmitted())
            m_program->Submit();
        for(auto& i : m_textures)
        {
            if(i && !i->IsSubmitted())
                i->Submit();
        }

        UpdateData(
            *m_mesh,
            m_textures,
            *m_program,
            m_args,
            m_settings
        );

        DataSubmitted();
    }

    void IDrawCall::SetMesh(IMesh* mesh)
    {
        assert(mesh);
        m_mesh = mesh;
        m_is_submitted = false;
    }
    void IDrawCall::SetShaderProgram(IShaderProgram* program)
    {
        assert(program);
        m_program = program;
        m_is_submitted = false;
    }
    ITexture2D* IDrawCall::GetTexture(std::size_t unit) noexcept
    {
        return unit < m_textures.size() ? m_textures[unit] : nullptr;
    }
    void IDrawCall::SetTexture(ITexture2D* texture, std::size_t unit)
    {
        if(m_textures.size() < unit + 1)
        {
            if(unit > GetRenderer().MaxTextureUnits())
            {
                throw std::invalid_argument("unit > renderer.MaxTextureUnits()");
            }
            m_textures.resize(unit + 1);
        }
        m_textures[unit] = texture;
    }

    void IDrawCall::DataSubmitted()
    {
        m_is_submitted = true;
    }
}
