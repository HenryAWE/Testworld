// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_GRAPHIC_HPP
#define TESTWORLD_GRAPHIC_GRAPHIC_HPP

#include <memory>
#include "../sys/init.hpp"
#include "renderer.hpp"


namespace awe::graphic
{
    std::unique_ptr<Renderer> CreateRenderer(
        const AppInitData& initdata,
        window::Window& window
    );
}


#endif
