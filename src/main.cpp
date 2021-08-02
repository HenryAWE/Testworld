// Author: HenryAWE
// License: The 3-clause BSD License

#include <memory>
#include <SDL.h>
#include "app.hpp"
#include "sys/init.hpp"


int SDL_main(int argc, char* argv[])
{
    awe::InitSDL();

    auto& app = awe::App::GetInstance();
    app.CreateWindow();

    app.Mainloop();

    app.Quit();

    awe::QuitSDL();
    return 0;
}