// Author: HenryAWE
// License: The 3-clause BSD License

#include "lang.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <imgui.h>
#include <rapidjson/document.h>
#include "../res/vfs.hpp"


namespace awe
{
    namespace detailed
    {
        bool Mount(const std::string& pakname)
        {
            namespace fs = std::filesystem;
            auto pakpath = fs::current_path() / "lang" / (pakname + ".pak");
            if(!fs::exists(pakpath))
                return false;
            int r = PHYSFS_mount(
                pakpath.u8string().c_str(),
                ("lang/" + pakname).c_str(),
                1
            );
            if(!r)
            {
                return false;
            }

            return true;
        }

        std::string GetString(rapidjson::Value& value)
        {
            return std::string(value.GetString(), value.GetStringLength());
        }
    }

    LangPak::LangPak() {}
    LangPak::~LangPak() {}

    bool LangPak::Load(const std::string& pakname)
    {
        if(!detailed::Mount(pakname))
            return false;

        m_pakname = pakname;
        auto data = vfs::GetString(GetFullpath("lang.json"));

        m_doc.Parse(data.c_str());
        auto& lang = m_doc["lang"];
        if(lang.IsNull())
            return false;
        m_id = detailed::GetString(lang["id"]);
        if(lang.HasMember("font"))
            m_font = GetFullpath(detailed::GetString(lang["font"]));
    }

    std::string LangPak::GetFullpath(const std::string& file)
    {
        return "lang/" + m_pakname + "/" + file;
    }

    bool LangPak::HasFont()
    {
        return m_font.has_value();
    }
    const std::string& LangPak::GetFontVfsPath()
    {
        return *m_font;
    }

    void LangPak::AddFont()
    {
        ImFontConfig cfg{};
        // Avoid double deletion
        cfg.FontDataOwnedByAtlas = false;
        
        auto ftdata = vfs::GetData(GetFontVfsPath());
        auto& fonts = ImGui::GetIO().Fonts;

        auto& lang_doc = m_doc["lang"];

        auto range_id = detailed::GetString(lang_doc["range"]);
        ImFontGlyphRangesBuilder range_builder;
        if(range_id == "Default")
            range_builder.AddRanges(fonts->GetGlyphRangesDefault());
        else if(range_id == "SimpChinese")
            range_builder.AddRanges(fonts->GetGlyphRangesChineseSimplifiedCommon());
        else
            range_builder.AddRanges(fonts->GetGlyphRangesDefault());

        float size = lang_doc["size"].GetFloat();
        size = std::clamp(size, 13.0f, 40.0f);
        range_builder.BuildRanges(&m_glyph_range);
        fonts->AddFontFromMemoryTTF(
            ftdata.data(),
            (int)ftdata.size(),
            size,
            &cfg,
            m_glyph_range.Data
        );
    }
}
