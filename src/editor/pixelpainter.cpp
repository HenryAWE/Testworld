// Author: HenryAWE
// License: The 3-clause BSD License

#include "pixelpainter.hpp"
#include <filesystem>
#include <stdexcept>
#include <imgui.h>
#include <ImGuiFileDialog.h>
#define IMGUI_DEFINE_MATH_OPERATORS 1
#include <imgui_internal.h>
#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <stb_image_write.h>


namespace awe
{
    namespace detailed
    {
        std::size_t CalcIndex(
            glm::ivec2 coord,
            glm::ivec2 size
        ) {
            if(coord[0] >= size[0] || coord[1] >= size[1])
                return -1;
            return coord[1] * size[0] + coord[0];
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
        if(!m_open)
            return;

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
        ImGui::BeginChild("##images", ImVec2(0, -22));
        if(m_bm_data.empty())
            ImGui::TextDisabled("(empty)");
        else
        {
            if(ImGui::BeginTabBar("##images_tabbar"))
            {
                std::size_t current = -1;
                for(std::size_t i = 0; i < m_bm_data.size(); ++i)
                {
                    ImGui::PushID(static_cast<int>(i));
                    bool tabitem_open = true;
                    if(ImGui::BeginTabItem(m_bm_data[i].name.c_str(), &tabitem_open))
                    {
                        current = i;
                        Canvas(i);
                        ImGui::EndTabItem();
                    }
                    ImGui::PopID();
                    if(!tabitem_open && !m_bm_data[i].saved)
                    {
                        m_popup = [this, i]()
                        {
                            ImGui::TextColored(
                                ImVec4(1, 0, 0, 1),
                                "\"%s\" is unsaved\n"
                                "Do you want to close it WITHOUT SAVING it?",
                                m_bm_data[i].name.c_str()
                            );
                            if(ImGui::Button("Confirm"))
                            {
                                m_bm_data.erase(m_bm_data.begin() + i);
                                ImGui::CloseCurrentPopup();
                                m_popup = std::function<void()>();
                            }
                            ImGui::SameLine();
                            if(ImGui::Button("Cancel"))
                            {
                                ImGui::CloseCurrentPopup();
                                m_popup = std::function<void()>();
                            }
                        };
                    }
                }
                ImGui::EndTabBar();
                m_current_bm_idx = current;
            }
        }
        ImGui::EndChild();
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

        const int filedlg_flags = 32 |
            ImGuiWindowFlags_NoSavedSettings;
        auto filedlg = ImGuiFileDialog::Instance();
        if(filedlg->Display("#pixelpainter_saveas", filedlg_flags, ImVec2(400, 300)))
        {
            if(filedlg->IsOk())
            {
                if(m_current_bm_idx != -1)
                {
                    using std::filesystem::path;

                    auto& bm = m_bm_data[m_current_bm_idx];
                    auto file =
                        path(filedlg->GetCurrentPath()) /
                        path(filedlg->GetCurrentFileName());
                    stbi_write_bmp(
                        file.u8string().c_str(),
                        bm.size[0], bm.size[1],
                        STBI_rgb_alpha,
                        bm.data.data()
                    );
                    bm.file = file;
                }
            }
            filedlg->Close();
        }
    }

    glm::u8vec4 PixelPainter::GetColor(
            std::size_t index,
            glm::ivec2 coord
    ) {
        auto& bm = m_bm_data[index];
        std::size_t data_index = detailed::CalcIndex(coord, bm.size);
        if(index == -1)
            throw std::out_of_range("Out of range");

        return bm.data[data_index];
    }

    void PixelPainter::Show() noexcept
    {
        m_open = true;
    }
    void PixelPainter::Hide() noexcept
    {
        m_open = false;
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
    void PixelPainter::Canvas(std::size_t id)
    {
        const int flags =
            ImGuiWindowFlags_NoMove;

        if(ImGui::BeginChild("##canvas", ImVec2(0, 0), true, flags))
        {
            auto& tex = GetCachedTex(id);
            ImVec2 im_size(
                static_cast<float>(tex.GetSize()[0]),
                static_cast<float>(tex.GetSize()[1])
            );
            ImGui::Image(
                reinterpret_cast<void*>((std::ptrdiff_t)tex.GetHandle()),
                im_size * m_factor
            );
            if(!m_bm_data[id].file.empty())
                ImGui::TextDisabled("Path: %s", m_bm_data[id].file.u8string().c_str());
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

        if(ImGui::Button("R"))
        {
            m_factor = 1.0f;
        }
        if(ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Reset Factor");
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120.0f);
        ImGui::SliderFloat(
            "Factor",
            &m_factor,
            0.5f, 25.0f, "%.01f"
        );
        ImGui::EndChild();
    }

    void PixelPainter::MenuBar_File()
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New...", "Ctrl+N"))
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

                    bool valid = true;
                    if(buf[0] == '\0')
                    {
                        ImGui::TextColored(
                            ImVec4(1, 0, 0, 1),
                            "Name cannot be empty"
                        );
                        valid = false;
                    }
                    if(size[0] > 256 || size[1] > 256)
                    {
                        ImGui::TextColored(
                            ImVec4(1, 0, 0, 1),
                            "Size cannot be bigger than 256*256"
                        );
                        valid = false;
                    }
                    if(ImGui::Button("Confirm") && valid)
                    {
                        NewBitmap(buf, size);
                        ImGui::CloseCurrentPopup();
                        m_popup = std::function<void()>();
                    }
                    ImGui::SameLine();
                    if(ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        m_popup = std::function<void()>();
                    }
                };
            }

            if(ImGui::MenuItem("Save", "Ctrl+S", nullptr, !m_bm_data.empty()))
            {
                auto& bm = m_bm_data[m_current_bm_idx];
                if(bm.file.empty())
                    OpenDlg_SaveAs(m_current_bm_idx);
                else
                {
                    stbi_write_bmp(
                        bm.file.u8string().c_str(),
                        bm.size[0], bm.size[1],
                        STBI_rgb_alpha,
                        bm.data.data()
                    );
                }
            }
            if(ImGui::MenuItem("Save As...", "Ctrl+Shift+S", nullptr, !m_bm_data.empty()))
            {
                OpenDlg_SaveAs(m_current_bm_idx);
            }
            ImGui::EndMenu();
        }
        
    }

    void PixelPainter::OpenDlg_SaveAs(std::size_t id)
    {
        using namespace std::filesystem;
        ImGuiFileDialog::Instance()->OpenModal(
            "#pixelpainter_saveas",
            "Save As",
            ".bmp,.png,.dat",
            (current_path() / m_bm_data[id].name).u8string()
        );
    }

    void PixelPainter::NewBitmap(const std::string& name, glm::ivec2 size)
    {
        BitmapData bm;
        bm.name = name;
        bm.size = size;
        bm.saved = false;
        bm.data.resize(size[0] * size[1], glm::u8vec4(255));

        m_bm_data.push_back(std::move(bm));
    }

    Texture& PixelPainter::GetCachedTex(std::size_t id)
    {
        auto iter = m_cache.find(id);
        if(iter != m_cache.end())
            return iter->second;

        auto& bm = m_bm_data[id];
        Texture tex;
        tex.LoadMemory(
            glm::value_ptr(bm.data[0]),
            bm.size,
            true
        );
        return m_cache.emplace(
            std::make_pair(id, std::move(tex))
        ).first->second;
    }
}
