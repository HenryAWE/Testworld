// Author: HenryAWE
// License: The 3-clause BSD License

#include "pixelpainter.hpp"
#include <stdexcept>
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS 1
#include <imgui_internal.h>
#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>


namespace awe
{
    namespace detailed
    {
        std::size_t CalcIndex(
            glm::ivec2 coord,
            std::size_t side_count
        ) {
            if(coord[0] >= side_count || coord[1] >= side_count)
                return -1;
            return coord[1] * side_count + coord[0];
        }

        enum ToolId : int
        {
            TOOLID_SELECT = 0,
            TOOLID_PEN = 1,
            TOOLID_ERASER = 2
        };

        const char* GetToolName(int id)
        {
            switch(id)
            {
                case TOOLID_SELECT: return "Select";
                case TOOLID_PEN: return "Pen";
                case TOOLID_ERASER: return "Eraser";
                default:
                    assert(0);
                    return "";
            }
        }
    }

    PixelPainter::PixelPainter()
    {
        m_color[0] = glm::vec4(0, 0, 0, 1);
        m_color[1] = glm::vec4(1);
    }

    void PixelPainter::NewFrame()
    {
        auto& io = ImGui::GetIO();

        const int flags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoSavedSettings;

        ImGui::SetNextWindowSizeConstraints(
            ImVec2(400, 300),
            ImVec2(INT16_MAX, INT16_MAX)
        );
        ImGui::SetNextWindowSize(
            io.DisplaySize * 0.8f,
            ImGuiCond_Appearing
        );
        if(!ImGui::Begin("Pixel Painter", &m_open, flags))
        {
            ImGui::End();
            return;
        }

        MenuBar();
        Toolbox();
        ImGui::SameLine();
        Canvas();
        BottomToolbox();

        // Popup window
        if(m_popup)
            ImGui::OpenPopup("##popup");
        const int popup_flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove;
        if(ImGui::BeginPopupModal("##popup", nullptr, popup_flags))
        {
            if(m_popup)
                m_popup();
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    glm::u8vec4 PixelPainter::GetColor(glm::ivec2 coord)
    {
        std::size_t index = detailed::CalcIndex(coord, m_side);
        if(index == -1)
            throw std::out_of_range("Out of range");

        return m_bitmap[index];
    }

    void PixelPainter::MenuBar()
    {
        if(!ImGui::BeginMenuBar())
            return;

        MenuBar_File();

        ImGui::EndMenuBar();
    }
    void PixelPainter::Toolbox()
    {
        using namespace std;

        ImVec2 size(50.0f, -22.0f);
        if(!ImGui::BeginChild("##toolbox", size, false))
        {
            ImGui::EndChild();
            return;
        }

        using detailed::ToolId;
        // tuple<name, callback>
        typedef
            tuple<ToolId, function<void()>>
            ToolboxButton;

        auto set_id = [this](ToolId id){ m_current_tool_id = id; };
        ToolboxButton buttons[] =
        {
            { ToolId::TOOLID_SELECT, bind(set_id, ToolId::TOOLID_SELECT) },
            { ToolId::TOOLID_PEN, bind(set_id, ToolId::TOOLID_PEN) },
            { ToolId::TOOLID_ERASER, bind(set_id, ToolId::TOOLID_ERASER) }
        };
        for(const auto& i : buttons)
        {
            const char* label = detailed::GetToolName(get<0>(i));
            if(ImGui::Button(label, ImVec2(48.0f, 0.0f)))
            {
                if(get<1>(i)) get<1>(i)();
            }
        }

        ImGui::Separator();
        ImGui::Text("Color");
        // tuple<label, index>
        tuple<const char*, int> colors[2] =
        {
            { "#1##color", 0 },
            { "#2##color", 1 }
        };
        for(const auto& i : colors)
        {
            ImGui::SetNextItemWidth(48.0f);
            ImGui::ColorEdit4(
                get<0>(i),
                glm::value_ptr(m_color[get<1>(i)]),
                ImGuiColorEditFlags_NoInputs
            );
        }

        ImGui::EndChild();
    }
    void PixelPainter::Canvas()
    {
        const int flags =
            ImGuiWindowFlags_NoMove;

        ImVec2 size(0.0f, -22.0f);
        if(!ImGui::BeginChild("##canvas", size, true, flags))
        {
            ImGui::EndChild();
            return;
        }


        ImGui::EndChild();
    }
    void PixelPainter::BottomToolbox()
    {
        const int flags = 
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;
        if(!ImGui::BeginChild("##bottom", ImVec2(0, 18), false, flags))
        {
            ImGui::EndChild();
            return;
        }

        ImGui::Text(
            "Current: %s",
            detailed::GetToolName(m_current_tool_id)
        );
        ImGui::SameLine();

        ImGui::SetNextItemWidth(120.0f);
        ImGui::SliderFloat(
            "Factor",
            &m_factor,
            0.1f, 10.0f, "%.01f"
        );
        ImGui::EndChild();
    }

    void PixelPainter::MenuBar_File()
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New..."))
            {
                m_popup = [this]()
                {
                    static char buf[128] = "New Bitmap";
                    ImGui::InputText(
                        "Name",
                        buf,
                        128
                    );
                    static glm::ivec2 size = { 16, 16 };
                    ImGui::InputInt2("Size", glm::value_ptr(size));
                    if(ImGui::Button("Confirm"))
                    {
                        NewBitmap(buf, size);
                        std::memset(buf, 0, 128);
                        ImGui::CloseCurrentPopup();
                        m_popup = std::function<void()>();
                    }
                };
            }
            ImGui::EndMenu();
        }
    }

    void PixelPainter::NewBitmap(const std::string& name, glm::ivec2 size)
    {
        //TODO
    }
}
