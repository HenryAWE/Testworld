// Author: HenryAWE
// License: The 3-clause BSD License

#include "init.hpp"
#include <SDL.h>
#include <stdexcept>


namespace awe
{
    void InitSDL()
    {
        constexpr int required_subsystems =
            SDL_INIT_AUDIO |
            SDL_INIT_VIDEO;

        if(SDL_Init(required_subsystems) < 0)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void QuitSDL() noexcept
    {
        SDL_Quit();
    }
}
