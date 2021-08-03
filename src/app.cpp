// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "app.hpp"
#include "renderer/renderer.hpp"


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
        m_renderer = std::make_shared<Renderer>(
            *m_window
        );
        m_renderer->CreateContext();
    }

    void App::Mainloop()
    {
        bool quit = false;
        while(!quit)
        {
            // Event processing
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

            // Rendering
            glClear(GL_COLOR_BUFFER_BIT);
            m_renderer->Present();
        }
    }

    void App::Quit()
    {
        m_renderer.reset();
        m_window.reset();
    }
}
