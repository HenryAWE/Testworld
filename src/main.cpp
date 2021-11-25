// Author: HenryAWE
// License: The 3-clause BSD License

#include <memory>
#include <SDL.h>
#include <physfs.h>
#include "app.hpp"
#include "sys/init.hpp"
#include "sys/singleinstance.hpp"
#include "res/vfs.hpp"


#ifndef SDL_main
#   define SDL_main main
#endif
int SDL_main(int argc, char* argv[])
{
    using namespace awe;
    if(!SingleInstance::GetInstance().Acquire())
        return EXIT_SUCCESS;

    Prepare(argv[0]);
    InitSDL();
    InitPhysfs(argv[0]);

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

    std::string arg1 = argc > 1 ? argv[1] : "en-US";

    auto& app = App::GetInstance();
    app.LoadLanguagePak(arg1);
    app.Initialize();

    app.Mainloop();

    app.Deinitialize();

    QuitPhysfs();
    QuitSDL();
    return 0;
}
