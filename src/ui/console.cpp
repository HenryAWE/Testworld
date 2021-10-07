// Author: HenryAWE
// License: The 3-clause BSD License

#include "console.hpp"
#include <SDL.h>
#include <scripthelper/scripthelper.h>
#include "../script/callconv.hpp"


namespace awe::imgui
{
    namespace detailed
    {
        static Console* instance = nullptr;

        void TW_CDECL Echo(const std::string& str)
        {
            if(!instance)
                return;
            instance->Write(str);
        }

        void TW_CDECL Exit()
        {
            SDL_QuitEvent quit{ SDL_QUIT, SDL_GetTicks() };
            SDL_PushEvent((SDL_Event*)&quit);
        }

        int RegisterConsoleInterface(asIScriptEngine* engine)
        {
            int r = 0;
            r = engine->RegisterGlobalFunction("void Echo(const string& in)", asFUNCTION(Echo), asCALL_CDECL);
            if(r < 0) return r;
            r = engine->RegisterGlobalFunction("void Exit()", asFUNCTION(Exit), asCALL_CDECL);
            if(r < 0) return r;

            return asSUCCESS;
        }
    }

    Console::Console()
    {
        detailed::instance = this;
    }
    Console::~Console()
    {
        detailed::instance = nullptr;
    }

    void Console::NewFrame()
    {
        const int generic_flags =
            ImGuiWindowFlags_NoSavedSettings;

        bool open = false;
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Appearing);
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(400, 300),
            ImVec2(INT16_MAX, INT16_MAX)
        );
        if(m_fixed_mode)
        {
            const int fixed_flags =
                generic_flags |
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove;

            open = ImGui::Begin("Console", nullptr, fixed_flags);
        }
        else
        { // Windowed mode
            const int windowed_flags =
                generic_flags;
            open = ImGui::Begin("Console", &m_show, windowed_flags);
        }
        if(open)
        {
            if(ImGui::BeginChild("output", ImVec2(0, 0), true))
                OutputRegion();
            ImGui::EndChild();

            ImGui::Text("Input");
            ImGui::SameLine();
            const int input_flags =
                ImGuiInputTextFlags_EnterReturnsTrue;
            bool enter = ImGui::InputText("", m_input_buf, 512, input_flags);
            ImGui::SameLine();
            if(ImGui::Button("Enter"))
                enter = true;
            if(enter)
            {
                ExecuteString(m_as_engine, m_input_buf, m_mod, m_console_ctx);
                m_input_buf[0] = '\0';
            }
        }
        ImGui::End();
    }

    void Console::Write(std::string_view sv)
    {
        m_outputs.emplace_back(sv);
    }

    int Console::SetScriptEngine(asIScriptEngine* engine, CScriptBuilder* builder)
    {
        m_as_engine = engine;
        assert(m_as_engine);
        int r = 0;
        r = detailed::RegisterConsoleInterface(m_as_engine);
        if(r < 0) return r;
        r = builder->StartNewModule(m_as_engine, "Console");
        if(r < 0) return r;
        r = builder->BuildModule();
        if(r < 0) return r;

        m_mod = engine->GetModule("Console");
        assert(m_mod);

        m_console_ctx = engine->CreateContext();
        return asSUCCESS;
    }
    void Console::ReleaseScriptEngine()
    {
        m_console_ctx->Release();
        m_console_ctx = nullptr;
    }

    void Console::OutputRegion()
    {
        for(const auto& i : m_outputs)
            DrawLine(i);
    }

    void Console::DrawLine(const ConsoleOutputLine& line)
    {
        ImGui::TextUnformatted(
            line.c_str(),
            line.c_str() + line.size()
        );
    }
}
