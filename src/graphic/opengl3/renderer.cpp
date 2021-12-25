// Author: HenryAWE
// License: The 3-clause BSD License

#include "renderer.hpp"


namespace awe::graphic::opengl3
{
    Renderer::Renderer(window::Window& window)
        : graphic::Renderer(window) {}

    Renderer::~Renderer() noexcept = default;

    std::string Renderer::GetRendererName()
    {
        return "opengl3";
    }
}
