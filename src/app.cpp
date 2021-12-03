// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "app.hpp"
#include <stdexcept>
#include <stb_image.h> // set flip
#include <imgui_impl_sdl.h>
#include "editor/editor.hpp"
#include "graphic/renderer.hpp"
#include "res/vfs.hpp"
#include "script/script.hpp"
#include <glm/gtc/matrix_transform.hpp>


namespace awe
{
    App::App() = default;
    App::~App() = default;

    App& App::GetInstance()
    {
        static App instance{};
        return instance;
    }

    void App::Initialize(const AppInitData& initdata)
    {
        m_window = std::make_shared<Window>(
            "Testworld",
            640, 480,
            SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE
        );

        m_renderer = std::make_shared<graphic::Renderer>(
            *m_window
        );
        m_renderer->CreateContext(initdata.ogl_debug);
        m_renderer->AttachDebugCallback();
        SDL_LogInfo(
            SDL_LOG_CATEGORY_APPLICATION,
            m_renderer->RendererInfo().c_str()
        );
        m_renderer->SetVSync();

        m_imgui_ctx = ImGui::CreateContext();
        if(!ImGui_ImplSDL2_InitForOpenGL(m_window->GetHandle(), m_renderer->GetContext()))
        {
            throw std::runtime_error("ImGui_ImplSDL2_InitForOpenGL() failed");
        }
        m_renderer->InitImGuiImpl();

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
        m_console->Write(m_renderer->RendererInfo());
        m_console->Write("Testworld Angelscript Console");

        // ImGui fonts
        auto& fonts = io.Fonts;
        GetLanguagePak().AddFont();
        fonts->AddFontDefault();
        fonts->Build();

        PrepareScriptEnv();
    }

    void App::Deinitialize()
    {
        ClearScriptEnv();

        m_editor.reset();
        m_console.reset();

        m_renderer->ShutdownImGuiImpl();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext(m_imgui_ctx);
        m_imgui_ctx = nullptr;
        m_renderer.reset();
        m_window.reset();
    }

    void App::LoadLanguagePak(const std::string& pakname)
    {
        bool loaded = m_lang.Load(pakname);
        if(!loaded && pakname != "en-US")
            m_lang.Load("en-US");
        
    }

    void App::Mainloop()
    {
        auto& io = ImGui::GetIO();

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

        graphic::ShaderBuilder shbuilder;
        shbuilder.AddShaderFromVfs(GL_VERTEX_SHADER, "shader/rect2D.vs");
        shbuilder.AddShaderFromVfs(GL_FRAGMENT_SHADER, "shader/screen.fs");
        graphic::ShaderProgram screen_sh = shbuilder.Build().first;
        shbuilder.Clear();

        auto perlin = m_renderer->GeneratePerlinTexture2D(0.5f, 256);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
            m_renderer->ImGuiImplNewFrame();
            ImGui_ImplSDL2_NewFrame(m_window->GetHandle());
            ImGui::NewFrame();

            m_console->NewFrame();

            m_editor->NewFrame();
            if(EditorNewFrame) EditorNewFrame();

            // Rendering
            auto drawsize = m_renderer->GetDrawableSize();
            for(int i : { 0, 1 })
                io.DisplayFramebufferScale[i] = float(drawsize[i]) / io.DisplaySize[i];
            ImGui::Render();
            auto& fbo = m_renderer->GetFramebuffer();
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, drawsize[0], drawsize[1]);
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(screen_sh);
            m_renderer->DrawTexture(m_renderer->GetScreenTexture(), glm::mat4(1), true);
            glUseProgram(0);

            m_renderer->ImGuiImplRenderDrawData();
            m_renderer->Present();
            int err = glGetError();
            if(err != GL_NO_ERROR)
            {
                SDL_LogError(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "GL Error (%x): ",
                    err
                );
            }
            assert(glGetError() == GL_NO_ERROR);
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

        script::InitScriptEnv(m_as_engine);

        awe::script::RegisterEditor(m_as_engine, m_editor.get());

        m_as_builder = std::make_unique<CScriptBuilder>();
        r = m_console->SetScriptEngine(m_as_engine, m_as_builder.get());
        assert(r >= 0);

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
    }
    void App::ClearScriptEnv()
    {
        m_console->ReleaseScriptEngine();
        script::ClearScriptEnv(m_as_engine);
        m_as_builder.reset();
        m_as_engine->ShutDownAndRelease();
    }

    LangPak& App::GetLanguagePak()
    {
        return m_lang;
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
