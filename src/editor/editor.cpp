// Author: HenryAWE
// License: The 3-clause BSD License

#include "editor.hpp"
#include <fmt/format.h>
#include <SDL.h>
#include <angelscript.h>
#include "../sys/misc.hpp"


namespace awe
{
    Editor::Editor()
    {
        // Build title bar
        m_titlebar.AddText("Editor");
        m_titlebar.AddSeparator();
        auto& file = m_titlebar.AddMenu("File");
        file.AddMenuItem("Quit").Connect([]{
            SDL_QuitEvent quit{SDL_QUIT, SDL_GetTicks()};
            SDL_PushEvent((SDL_Event*)&quit);
        });
        auto& window = m_titlebar.AddMenu("Window");
        auto& help = m_titlebar.AddMenu("Help");
        help.AddMenuItem("Information").Connect([this]{
            m_infowin = true;
        });
        help.AddMenuItem("Homepage").Connect(
            std::bind(OpenUrl, "https://github.com/HenryAWE/Testworld")
        );
    }

    void Editor::NewFrame()
    {
        auto& io = ImGui::GetIO();

        const int background_flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoDecoration|
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoResize;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        if(ImGui::Begin("Editor###background", nullptr, background_flags))
        {
            TitleBar();
        }
        ImGui::End();

        if(m_infowin)
            InfoWin();
    }

    void Editor::ShowInfo()
    {
        m_infowin = true;
    }

    void Editor::TitleBar()
    {
        if(!ImGui::BeginMenuBar())
            return;
        m_titlebar.Run();
        ImGui::EndMenuBar();
    }

    void Editor::InfoWin()
    {
        const int flags = 
            ImGuiWindowFlags_NoSavedSettings;
        if(ImGui::Begin("Information", &m_infowin, flags))
        {
            auto& text = GetInfo();
            ImGui::TextUnformatted(
                text.c_str(),
                text.c_str() + text.size()
            );
        }
        ImGui::End();
    }

    const std::string& Editor::GetInfo()
    {
        if(!m_info)
        {
            m_info = fmt::format(
                "Dear ImGui {} ({})\n"
                "Angelscript {} - {}",
                IMGUI_VERSION, IMGUI_VERSION_NUM,
                asGetLibraryVersion(), asGetLibraryOptions()
            );
        }

        return *m_info;
    }
}
