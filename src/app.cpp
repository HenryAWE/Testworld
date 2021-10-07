// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "app.hpp"
#include <stdexcept>
#include <scriptarray/scriptarray.h>
#include <scriptstdstring/scriptstdstring.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "editor/editor.hpp"
#include "renderer/renderer.hpp"
#include "res/vfs.hpp"
#include "script/scriptutil.hpp"
#include "script/register.hpp"


void TW_CDECL print(const std::string& s)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", s.c_str());
}

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
            640, 480,
            SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE
        );
        m_renderer = std::make_shared<Renderer>(
            *m_window
        );
        m_renderer->CreateContext();
        SDL_LogInfo(
            SDL_LOG_CATEGORY_APPLICATION,
            m_renderer->RendererInfo().c_str()
        );
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

        auto& io = ImGui::GetIO();
        SDL_LogInfo(
            SDL_LOG_CATEGORY_APPLICATION,
            "ImGui Information\n"
            "Version: %s (%d)\n"
            "Platform: %s\n"
            "Renderer: %s",
            IMGUI_VERSION, IMGUI_VERSION_NUM,
            io.BackendPlatformName,
            io.BackendRendererName
        );

        // Create ImGui window
        m_console = std::make_unique<imgui::Console>();
        m_editor = std::make_unique<Editor>();
        m_console->Write("Testworld Angelscript Console");
    }

    void App::Mainloop()
    {
        SDL_LogInfo(
            SDL_LOG_CATEGORY_APPLICATION,
            "Begin mainloop"
        );

        asIScriptModule* testworld = m_as_engine->GetModule("Testworld");

        asIScriptContext* main_ctx = m_as_engine->CreateContext();
        
        auto Preload = script::GenCallerByDecl<void()>(testworld, "void Preload()", main_ctx);
        if(Preload) Preload();

        auto EditorBeginMainloop = script::GenCallerByDecl<void()>(testworld, "void EditorBeginMainloop()", main_ctx);
        if(EditorBeginMainloop) EditorBeginMainloop();
        auto EditorNewFrame = script::GenCallerByDecl<void()>(testworld, "void EditorNewFrame()", main_ctx);

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
                    if(BeforeQuit)
                        quit = BeforeQuit();
                    else
                        quit = true;
                    break;

                default:
                    ImGui_ImplSDL2_ProcessEvent(&event);
                    break;
                }
            }

            // UI Processing
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(m_window->GetHandle());
            ImGui::NewFrame();

            m_console->NewFrame();

            m_editor->NewFrame();
            if(EditorNewFrame) EditorNewFrame();

            // Rendering
            ImGui::Render();
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            m_renderer->Present();
        }

        main_ctx->Release();

        SDL_LogInfo(
            SDL_LOG_CATEGORY_APPLICATION,
            "Quit mainloop"
        );
    }

    void App::PrepareScriptEnv()
    {
        m_as_engine = asCreateScriptEngine();
        int r = 0;
        r = m_as_engine->SetMessageCallback(
            asMETHOD(App, MessageCallback),
            this,
            asCALL_THISCALL
        );
        RegisterScriptArray(m_as_engine, true);
        RegisterStdString(m_as_engine);
        RegisterStdStringUtils(m_as_engine);

        awe::script::RegisterEditor(m_as_engine, m_editor.get());

        r = m_as_engine->RegisterGlobalFunction(
            "void print(const string& in)",
            asFUNCTION(print),
            asCALL_CDECL
        );
        assert(r >= 0);

        m_as_builder = std::make_unique<CScriptBuilder>();
        // Build internal script
        m_as_builder->StartNewModule(m_as_engine, "Testworld");
        std::vector<std::string> script_srcs;
        vfs::EnumFiles("script", std::back_inserter(script_srcs));
        for(const auto& f : script_srcs)
        {
            r = script::AddSectionFromVfs(m_as_builder.get(), "script/" + f);
        }
        r = m_as_builder->BuildModule();
        assert(r >= 0);

        m_console->SetScriptEngine(m_as_engine, m_as_builder.get());
    }
    void App::ClearScriptEnv()
    {
        m_console->ReleaseScriptEngine();
        m_as_builder.reset();
        m_as_engine->ShutDownAndRelease();
    }

    void App::Quit()
    {
        m_editor.reset();
        m_console.reset();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext(m_imgui_ctx);
        m_imgui_ctx = nullptr;
        m_renderer.reset();
        m_window.reset();
    }

    void App::MessageCallback(const asSMessageInfo* msg)
    {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[angelscript] %s (%d : %d)\n%s",
            msg->section,
            msg->row,
            msg->col,
            msg->message
        );
    }
}
