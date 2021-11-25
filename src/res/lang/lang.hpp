// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_LANG_LANG_HPP
#define TESTWORLD_LANG_LANG_HPP

#include <optional>
#include <string>
#include <rapidjson/document.h>
#include <imgui.h>


namespace awe
{
    class LangPak
    {
    public:
        LangPak();
        ~LangPak();

        // Mount and load language package
        bool Load(const std::string& pakname);

        std::string GetFullpath(const std::string& file);

        bool HasFont();
        const std::string& GetFontVfsPath();
        // Add the font to ImGui
        void AddFont();

    private:
        rapidjson::Document m_doc;

        std::string m_pakname;
        std::string m_id;
        std::optional<std::string> m_font;

        ImVector<ImWchar> m_glyph_range;
    };

    const ImWchar* GetFontRange(const std::string& lang);
}


#endif
