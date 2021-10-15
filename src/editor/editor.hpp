// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_EDITOR_EDITOR_HPP
#define TESTWORLD_EDITOR_EDITOR_HPP

#include <memory>
#include <optional>
#include <imgui.h>
#include "viewer.hpp"
#include "imageviewer.hpp"
#include "vfsviewer.hpp"


namespace awe
{
    class Editor
    {
    public:
        void NewFrame();

        ImageViewer& GetPixelPainter();

        void ShowInfo();

    private:
        void TitleBar();

        void InfoWin();
        bool m_infowin = false;

        std::unique_ptr<TextViewer> m_text_viewer;
        std::unique_ptr<ImageViewer> m_img_painter;
        std::unique_ptr<VfsViewer> m_vfs_viewer;

        std::optional<std::string> m_info;
        const std::string& GetInfo();
    };
}


#endif
