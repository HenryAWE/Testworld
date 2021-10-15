// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_EDITOR_IMAGEVIEWER_HPP
#define TESTWORLD_EDITOR_IMAGEVIEWER_HPP

#include <filesystem>
#include <functional>
#include <map>
#include <vector>
#include <tuple>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "../renderer/texture.hpp"


namespace awe
{
    class Texture;

    class ImageViewer
    {
    public:
        ImageViewer();

        void NewFrame();

        void Show() noexcept;
        void Hide() noexcept;
        [[nodiscard]]
        constexpr bool IsShow() const noexcept { return m_open; }

    private:

        /* Children */
        void MenuBar();
        void Display(std::size_t id);
        void BottomToolbox();

        void MenuBar_File();

        void OpenDlg_Open(const std::filesystem::path& pt = std::filesystem::current_path());

        /* Bitmap data */
        struct BitmapData
        {
            std::string name;
            Texture tex;
            std::filesystem::path file;

            BitmapData(const std::filesystem::path& file_);
            BitmapData(BitmapData&& move) noexcept = default;
            BitmapData& operator=(BitmapData&& move) noexcept = default;
        };

        void OpenBitmap(const std::filesystem::path& file);
        void DestroyBitmap(std::size_t id);

        void SetPopup(const std::string& title, std::function<void()> callback);
        void ClearPopup();

        std::string m_popup_title;
        std::function<void()> m_popup;

        bool m_open = true;
        glm::vec2 m_canvas_size = glm::vec2(0);
        float m_factor = 1.0f;
        std::vector<BitmapData> m_bm_data; // Data of bitmaps
        std::size_t m_current_bm_idx = -1;
    };
}

#endif
