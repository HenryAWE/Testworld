// Author: HenryAWE
// License: The 3-clause BSD License

#include "imageviewer.hpp"
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
    ImageViewer::ImageViewer() = default;

    void ImageViewer::NewFrame()
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
        if(!ImGui::Begin("Image Viewer", &m_open, flags))
        {
            ImGui::End();
            return;
        }

        MenuBar();
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
                        Display(i);
                        ImGui::EndTabItem();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTabBar();
                m_current_bm_idx = current;
            }
        }
        ImGui::EndChild();
        BottomToolbox();

        // Popup window
        if(m_popup)
            ImGui::OpenPopup(m_popup_title.c_str());
        const int popup_flags =
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove;
        if(ImGui::BeginPopupModal(m_popup_title.c_str(), nullptr, popup_flags))
        {
            if(m_popup)
                m_popup();
            ImGui::EndPopup();
        }

        ImGui::End();

        const int filedlg_flags = 32 |
            ImGuiWindowFlags_NoSavedSettings;
        auto filedlg = ImGuiFileDialog::Instance();
        if(filedlg->Display("#imageviewer_open", filedlg_flags, ImVec2(400, 300)))
        {
            if(filedlg->IsOk())
            {
                using namespace std::filesystem;

                path file =
                    filedlg->GetCurrentPath() + '/' +
                    filedlg->GetCurrentFileName();
                OpenBitmap(file);
            }
            filedlg->Close();
        }
    }

    void ImageViewer::Show() noexcept
    {
        m_open = true;
    }
    void ImageViewer::Hide() noexcept
    {
        m_open = false;
    }

    void ImageViewer::MenuBar()
    {
        if(!ImGui::BeginMenuBar())
            return;

        MenuBar_File();

        ImGui::EndMenuBar();
    }
    void ImageViewer::Display(std::size_t id)
    {
        const int flags =
            ImGuiWindowFlags_HorizontalScrollbar |
            ImGuiWindowFlags_NoMove;

        if(ImGui::BeginChild("##canvas", ImVec2(0, 0), true, flags))
        {
            auto& tex = m_bm_data[id].tex;
            ImVec2 im_size(
                static_cast<float>(tex.GetSize()[0]),
                static_cast<float>(tex.GetSize()[1])
            );
            ImGui::Image(
                reinterpret_cast<void*>((std::ptrdiff_t)tex.GetHandle()),
                im_size * m_factor,
                ImVec2(0, 1),
                ImVec2(1, 0)
            );

            ImVec2 im_canvas_size = ImGui::GetWindowContentRegionMax();
            m_canvas_size = glm::vec2(
                im_canvas_size[0],
                im_canvas_size[1]
            );
        }

        ImGui::EndChild();
    }
    void ImageViewer::BottomToolbox()
    {
        const int flags = 
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;
        if(!ImGui::BeginChild("##bottom", ImVec2(0, 18), false, flags))
        {
            ImGui::EndChild();
            return;
        }

        if(ImGui::Button("R"))
        {
            m_factor = 1.0f;
        }
        if(ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Reset Factor");
        }
        ImGui::SameLine();
        if(ImGui::Button("A") && !m_bm_data.empty())
        {
            glm::vec2 factor(m_factor);
            for(int i : {0, 1})
            {
                factor[i] =
                    m_canvas_size[i] /
                    float(m_bm_data[m_current_bm_idx].tex.GetSize()[i]);
                factor[i] = std::clamp(factor[i], 0.1f, 25.0f);
            }
            m_factor = std::min(factor[0], factor[1]);
        }
        if(ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Adjust Factor By Canvas Size");
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120.0f);
        ImGui::SliderFloat(
            "Factor",
            &m_factor,
            0.1f, 25.0f, "%.01f"
        );

        if(m_current_bm_idx != -1)
        {
            auto& filepath = m_bm_data[m_current_bm_idx].file;
            if(!filepath.empty())
            {
                ImGui::SameLine();
                ImGui::TextDisabled("Path: %s", filepath.u8string().c_str());
            }
        }

        ImGui::EndChild();
    }

    void ImageViewer::MenuBar_File()
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open...", "Ctrl+O"))
            {
                OpenDlg_Open();
            }
            ImGui::EndMenu();
        }
    }

    void ImageViewer::OpenDlg_Open(const std::filesystem::path& pt)
    {
        ImGuiFileDialog::Instance()->OpenModal(
            "#imageviewer_open",
            "Open",
            ".bmp,.png,.dat,.jpg",
            pt.u8string(),
            1,
            nullptr,
            ImGuiFileDialogFlags_DisableCreateDirectoryButton
        );
    }

    ImageViewer::BitmapData::BitmapData(const std::filesystem::path& file_)
    {
        name = file_.filename().u8string();
        file = file_;
        tex.LoadFile(file_);
    }

    void ImageViewer::OpenBitmap(const std::filesystem::path& file)
    {
        m_bm_data.emplace_back(file);
    }
    void ImageViewer::DestroyBitmap(std::size_t id)
    {
        assert(id < m_bm_data.size());
        m_bm_data.erase(m_bm_data.begin() + id);

        // Reset index
        if(m_current_bm_idx == id)
            m_current_bm_idx = -1;
    }

    void ImageViewer::SetPopup(const std::string& title, std::function<void()> callback)
    {
        m_popup_title = title + "###popup";
        m_popup = std::move(callback);
    }
    void ImageViewer::ClearPopup()
    {
        m_popup = std::function<void()>();
    }
}
