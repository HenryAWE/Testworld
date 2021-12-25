// Author: HenryAWE
// License: The 3-clause BSD License

#include "graphic.hpp"
#include "opengl3/renderer.hpp"


namespace awe::graphic
{
    std::unique_ptr<Renderer> CreateRenderer(
        const AppInitData& initdata,
        window::Window& window
    ) {
        return std::make_unique<opengl3::Renderer>(window);
    }
}
