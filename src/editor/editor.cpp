// Author: HenryAWE
// License: The 3-clause BSD License

#include "editor.hpp"
#include <fmt/format.h>
#include <SDL.h>
#include <angelscript.h>


namespace awe
{
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

        if(m_px_painter)
            m_px_painter->NewFrame();
        if(m_infowin)
            InfoWin();
    }

    PixelPainter& Editor::GetPixelPainter()
    {
        if(!m_px_painter)
            m_px_painter = std::make_unique<PixelPainter>();
        return *m_px_painter;
    }

    void Editor::ShowInfo()
    {
        m_infowin = true;
    }

    void Editor::TitleBar()
    {
        if(!ImGui::BeginMenuBar())
            return;
        ImGui::Text("Editor");
        ImGui::Separator();
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Quit"))
            {
                SDL_QuitEvent quit{SDL_QUIT, SDL_GetTicks()};
                SDL_PushEvent((SDL_Event*)&quit);
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Window"))
        {
            if(ImGui::MenuItem("Pixel Painter"))
            {
                GetPixelPainter().Show();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Help"))
        {
            if(ImGui::MenuItem("Information"))
                m_infowin = true;
            ImGui::EndMenu();
        }
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
