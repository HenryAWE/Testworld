// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_HPP

#include <glad/glad.h>
#include "../mesh.hpp"


namespace awe::graphic::opengl3
{
    class Renderer;

    class Mesh : public graphic::Mesh
    {
    public:
        typedef graphic::Mesh Super;

        Mesh(Renderer& renderer, bool dynamic = false);

        void Submit() override;
        void Draw() override;

        [[nodiscard]]
        Renderer& GetRenderer() noexcept;

    private:
        void Initialize();
        void Deinitialize();

        bool m_init = false;
        struct GLData
        {
            GLuint vao = 0;
            GLuint vbo = 0;
            GLuint ebo = 0;
        };
        GLData m_gldata;
        struct DrawCfg
        {
            GLenum mode;
            GLsizei count;
            GLenum type;
        };
        DrawCfg m_drawcfg;
    };
}


#endif
