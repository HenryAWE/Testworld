// Author: HenryAWE
// License: The 3-clause BSD License


#ifndef TESTWORLD_GRAPHIC_OPENGL3_DRAWCALL_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_DRAWCALL_HPP

#include <glad/glad.h>
#include "glutil.hpp"
#include "../drawcall.hpp"


namespace awe::graphic::opengl3
{
    class Renderer;
    class Mesh;
    class ShaderProgram;
    class Texture2D;

    class GLShaderUniformArg
    {
    public:
        GLint location;
        ShaderUniformArg::ArgType arg;
    };

    class DrawCall : public IDrawCall
    {
        typedef IDrawCall Super;
    public:
        DrawCall(Renderer& renderer);

        void Draw() override;

        Mesh* GetMesh() noexcept;
        ShaderProgram* GetShaderProgram() noexcept;
        Texture2D* GetTexture(std::size_t unit) noexcept;

    protected:
        void UpdateData(
            IMesh& mesh,
            std::span<ITexture2D*> textures,
            IShaderProgram& program,
            std::span<ShaderUniformArg> args,
            const DrawCallSettings& settings
        ) override;

    private:
        std::vector<GLShaderUniformArg> m_glargs;
    };
}


#endif
