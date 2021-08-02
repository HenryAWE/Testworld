// Author: HenryAWE
// License: The 3-clause BSD License

#include "app.hpp"


namespace awe
{
    App::App() = default;
    App::~App() = default;

    App& App::GetInstance()
    {
        static App instance{};
        return instance;
    }

    void App::CreateWindow()
    {
        m_window = std::make_shared<Window>(
            "Testworld",
            640, 480
        );
    }

    void App::Mainloop()
    {
        bool quit = false;
        while(!quit)
        {
            SDL_Event event{};
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;

                default:
                    break;
                }
            }
        }
    }

    void App::Quit()
    {
        m_window.reset();
    }
}