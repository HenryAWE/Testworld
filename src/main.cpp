// Author: HenryAWE
// License: The 3-clause BSD License

#include <memory>
#include <SDL.h>
#include <physfs.h>
#include "app.hpp"
#include "sys/init.hpp"
#include "sys/singleinstance.hpp"


#ifndef SDL_main
#   define SDL_main main
#endif
int SDL_main(int argc, char* argv[])
{
    if(!awe::SingleInstance::GetInstance().Acquire())
        return EXIT_SUCCESS;

    awe::Prepare(argv[0]);
    awe::InitSDL();
    awe::InitPhysfs(argv[0]);

    namespace fs = std::filesystem;
    PHYSFS_mount(
        fs::current_path().u8string().c_str(),
        "app",
        true
    );
    const std::string packages[] =
    {
        "resource",
        "script",
        "shader"
    };
    for(auto& i : packages)
    {
        auto name = i + ".pak";
        if(fs::exists(name))
        {
            int r = PHYSFS_mount(
                name.c_str(),
                i.c_str(),
                true
            );
            if(!r)
            {
                SDL_LogError(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "Failed to mount \"%s\" to \"%s\": %s",
                    name.c_str(),
                    i.c_str(),
                    PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
                );
            }
        }
    }

    auto& app = awe::App::GetInstance();
    app.CreateWindow();
    app.PrepareScriptEnv();

    app.Mainloop();

    app.ClearScriptEnv();
    app.Quit();

    awe::QuitPhysfs();
    awe::QuitSDL();
    return 0;
}
