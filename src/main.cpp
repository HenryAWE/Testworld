// Author: HenryAWE
// License: The 3-clause BSD License

#include <memory>
#include <SDL.h>
#include <physfs.h>
#include "app.hpp"
#include "sys/init.hpp"


#ifndef SDL_main
#   define SDL_main main
#endif
int SDL_main(int argc, char* argv[])
{
    awe::Prepare(argv[0]);
    awe::InitSDL();
    awe::InitPhysfs(argv[0]);

    namespace fs = std::filesystem;
    PHYSFS_mount(
        fs::current_path().u8string().c_str(),
        "app",
        true
    );
    if(fs::exists("script.pak"))
    {
        PHYSFS_mount(
            "script.pak",
            "script",
            true
        );
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
