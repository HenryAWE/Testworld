// Author: HenryAWE
// License: The 3-clause BSD License

#include <memory>
#include <SDL.h>
#include <physfs.h>
#include "app.hpp"
#include "sys/init.hpp"
#include "sys/singleinstance.hpp"
#include "res/res.hpp"


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

    res::Initialize();
    std::string arg1 = argc > 1 ? argv[1] : "en-US";

    auto& app = App::GetInstance();
    app.LoadLanguagePak(arg1);
    app.Initialize();

    app.Mainloop();

    app.Deinitialize();

    res::Deinitialize();

    QuitPhysfs();
    QuitSDL();
    return 0;
}
