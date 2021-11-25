// Author: HenryAWE
// License: The 3-clause BSD License

#include <iostream>
#include <memory>
#include <SDL.h>
#include <physfs.h>
#include "app.hpp"
#include "cli/cli.hpp"
#include "sys/init.hpp"
#include "sys/singleinstance.hpp"
#include "res/res.hpp"


#ifndef SDL_main
#   define SDL_main main
#endif
int SDL_main(int argc, char* argv[])
{
    using namespace awe;

    cli::CommandlineInterface cli;
    cli.Parse(argc, argv);
    if(cli.HasError())
    {
        std::string err = cli.GetError();
        std::cerr << err << std::endl;
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "CLI Error!",
            err.c_str(),
            nullptr
        );
        return EXIT_FAILURE;
    }
    if(cli.Exists("help"))
    {
        std::string help = cli.GetHelp();
        std::cout << help << std::endl;
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_INFORMATION,
            "Help",
            help.c_str(),
            nullptr
        );
        return EXIT_SUCCESS;
    }
    if(!SingleInstance::GetInstance().Acquire())
        return EXIT_SUCCESS;

    Prepare(argv[0]);
    InitSDL();
    InitPhysfs(argv[0]);

    res::Initialize();
    std::string lang = cli.Exists("language") ?
        cli.GetVal<std::string>("language") :
        "en-US";

    auto& app = App::GetInstance();
    app.LoadLanguagePak(lang);
    app.Initialize();

    app.Mainloop();

    app.Deinitialize();

    res::Deinitialize();

    QuitPhysfs();
    QuitSDL();
    return 0;
}
