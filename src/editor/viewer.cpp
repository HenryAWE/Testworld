// Author: HenryAWE
// License: The 3-clause BSD License

#include "viewer.hpp"
#include <fstream>
#include <sstream>
#include <utility>
#include <fmt/format.h>
#define IMGUI_DEFINE_MATH_OPERATORS 1
#include <imgui_internal.h>


namespace awe
{
    TextViewer::TextViewer(
        std::string text
    ) : m_text(std::move(text))
    {
        m_id = fmt::format("Text Viewer###{id}", fmt::arg("id", (void*)this));
    }

    void TextViewer::NewFrame()
    {
        if(!m_open)
            return;

        auto& io = ImGui::GetIO();

        int flags = ImGuiWindowFlags_NoSavedSettings;

        ImGui::SetNextWindowSize(io.DisplaySize * 0.8f, ImGuiCond_Appearing);
        if(ImGui::Begin(m_id.c_str(), &m_open, flags))
        {
            ImGui::TextUnformatted(
                m_text.c_str(),
                m_text.c_str() + m_text.length()
            );
        }
        ImGui::End();
    }

    void TextViewer::SetText(std::string text)
    {
        m_text.swap(text);
    }
    bool TextViewer::LoadFile(const std::filesystem::path& file)
    {
        using namespace std;

        ifstream ifs(file);
        if(!ifs)
        {
            m_text.clear();
            return false;
        }
        stringstream ss;
        ss << ifs.rdbuf();

        SetText(ss.str());
        return true;
    }

    void TextViewer::Show() noexcept
    {
        m_open = true;
    }
    void TextViewer::Hide() noexcept
    {
        m_open = false;
    }
}
