// Author: HenryAWE
// License: The 3-clause BSD License

#include "drawcall.hpp"
#include <cassert>
#include "renderer.hpp"


namespace awe::graphic::opengl3
{
    namespace detailed
    {
        static void SetGLState(GLenum cap, bool value) noexcept
        {
            value ? glEnable(cap) : glDisable(cap);
        }
        static void ApplyStates(const DrawCallSettings& settings)
        {
            SetGLState(GL_DEPTH_TEST, settings.depth_test);
            bool cull_face = settings.culling != Culling::None;
            if(cull_face)
            {
                glEnable(GL_CULL_FACE);
                switch(settings.culling)
                {
                    case Culling::Front:
                        glCullFace(GL_FRONT);
                        break;

                    default:
                        assert(false);
                        [[fallthrough]];
                    [[likely]]
                    case Culling::Back:
                        glCullFace(GL_BACK);
                        break;
                }
            }
            else
                glDisable(GL_CULL_FACE);
            SetGLState(GL_BLEND, settings.blending);
        }

        static void BindTexture(Texture2D& tex, GLenum unit)
        {
            glActiveTexture( GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, tex);
        }
        static void UnbindTexture(GLenum unit, GLenum target)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(target, 0);
        }

        static void SetUniformArg(const GLShaderUniformArg& arg)
        {
            std::visit(
                [l = arg.location](auto&& arg) { Uniform(l, arg); },
                arg.arg
            );
        }
    }

    DrawCall::DrawCall(Renderer& renderer)
        : Super(renderer) {}

    void DrawCall::Draw()
    {
        if(!IsSubmitted())
            Submit();

        detailed::ApplyStates(GetSettings());
        glUseProgram(*GetShaderProgram());
        for(const auto& i : m_glargs)
            detailed::SetUniformArg(i);
        for(std::size_t i = 0; i < GetTextureCount(); ++i)
        {
            if(auto* tex = GetTexture(i); tex)
            {
                detailed::BindTexture(
                    *tex,
                    static_cast<GLenum>(i)
                );
            }
        }
        GetMesh()->Draw();
        for(std::size_t i = 0; i < GetTextureCount(); ++i)
        {
            if(auto* tex = GetTexture(i); tex)
            {
                detailed::UnbindTexture(
                    static_cast<GLenum>(i),
                    tex->GetTarget()
                );
            }
        }
        glUseProgram(0);
    }

    Mesh* DrawCall::GetMesh() noexcept
    {
        assert(dynamic_cast<Mesh*>(Super::GetMesh()));
        return static_cast<Mesh*>(Super::GetMesh());
    }
    ShaderProgram* DrawCall::GetShaderProgram() noexcept
    {
        assert(dynamic_cast<ShaderProgram*>(Super::GetShaderProgram()));
        return static_cast<ShaderProgram*>(Super::GetShaderProgram());
    }
    Texture2D* DrawCall::GetTexture(std::size_t unit) noexcept
    {
        auto* tex = Super::GetTexture(unit);
        if(!tex)
            return nullptr;
        assert(dynamic_cast<Texture2D*>(tex));
        return static_cast<Texture2D*>(tex);
    }

    void DrawCall::UpdateData(
        IMesh& mesh,
        std::span<ITexture2D*> textures,
        IShaderProgram& program,
        std::span<ShaderUniformArg> args,
        const DrawCallSettings& settings
    ) {
        m_glargs.clear();
        m_glargs.reserve(args.size());
        for(auto& i : args)
        {
            m_glargs.emplace_back(
                GetShaderProgram()->UniLoc(i.name.c_str()),
                std::move(i.arg)
            );
        }
    }
}
