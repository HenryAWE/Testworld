// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_EDITOR_EDITOR_HPP
#define TESTWORLD_EDITOR_EDITOR_HPP

#include <memory>
#include <optional>
#include <imgui.h>
#include "../ui/imgui.hpp"
#include "viewer.hpp"
#include "vfsviewer.hpp"


namespace awe
{
    class Editor
    {
    public:
        Editor();

        void NewFrame();

        void ShowInfo();

    private:
        void TitleBar();
        ui::ImGuiBuilder m_titlebar;

        void InfoWin();
        bool m_infowin = false;

        std::unique_ptr<TextViewer> m_text_viewer;
        std::unique_ptr<VfsViewer> m_vfs_viewer;

        std::optional<std::string> m_info;
        const std::string& GetInfo();
    };
}


#endif
