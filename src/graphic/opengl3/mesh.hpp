// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_HPP

#include <glad/glad.h>
#include "glutil.hpp"
#include "../mesh.hpp"


namespace awe::graphic::opengl3
{
    class Renderer;

    class Mesh : public IMesh
    {
        typedef IMesh Super;
    public:
        Mesh(Renderer& renderer, bool dynamic = false);

        ~Mesh() noexcept;

        void Submit() override;
        void Draw() override;

        [[nodiscard]]
        Renderer& GetRenderer() noexcept;

    private:
        void Initialize();
        void Deinitialize() noexcept;

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
