// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "app.hpp"
#include <stdexcept>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
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
        if(SDL_GL_SetSwapInterval(1) == -1)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_SetSwapInterval(1) failed");
        }

        m_imgui_ctx = ImGui::CreateContext();
        if(!ImGui_ImplSDL2_InitForOpenGL(m_window->GetHandle(), m_renderer->GetContext()))
        {
            throw std::runtime_error("ImGui_ImplSDL2_InitForOpenGL() failed");
        }
        if(!ImGui_ImplOpenGL3_Init("#version 330 core"))
        {
            throw std::runtime_error("ImGui_ImplOpenGL3_Init() failed");
        }
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

            // UI Processing
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(m_window->GetHandle());
            ImGui::NewFrame();

            // Rendering
            ImGui::Render();
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            m_renderer->Present();
        }
    }

    void App::Quit()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext(m_imgui_ctx);
        m_imgui_ctx = nullptr;
        m_renderer.reset();
        m_window.reset();
    }
}
