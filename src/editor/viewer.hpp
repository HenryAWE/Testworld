// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_EDITOR_VIEWER_HPP
#define TESTWORLD_EDITOR_VIEWER_HPP

#include <filesystem>
#include <string>
#include <imgui.h>


namespace awe
{
    class TextViewer
    {
    public:
        TextViewer(
            std::string text = std::string()
        );

        void NewFrame();

        void SetText(std::string text);
        bool LoadFile(const std::filesystem::path& file);
        [[nodiscard]]
        constexpr const std::string& GetText() const noexcept { return m_text; }

        void Show() noexcept;
        void Hide() noexcept;
        [[nodiscard]]
        constexpr bool IsShow() const noexcept { return m_open; }

    private:
        bool m_open = true;
        std::string m_id; // Format: title##id
        std::string m_text;
    };
}

#endif
