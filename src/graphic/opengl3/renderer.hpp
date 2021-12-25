// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_RENDERER_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_RENDERER_HPP


#include "glutil.hpp"
#include "../renderer.hpp"


namespace awe::graphic::opengl3
{
    /* OpenGL 3 Renderer */
    class Renderer : public graphic::Renderer
    {
    public:
        Renderer(window::Window& window);

        ~Renderer() noexcept;

        std::string GetRendererName() override;
    private:
    };
}


#endif
