// Author: HenryAWE
// License: The 3-clause BSD License

#include "res.hpp"
#include <filesystem>


namespace awe::res
{
    void Initialize()
    {
        namespace fs = std::filesystem;
        PHYSFS_mount(
            fs::current_path().u8string().c_str(),
            "app",
            true
        );
        const std::string packages[] =
        {
            "resource",
            "script",
            "shader"
        };
        for(auto& i : packages)
        {
            auto name = i + ".pak";
            if(fs::exists(name))
            {
                int r = PHYSFS_mount(
                    name.c_str(),
                    i.c_str(),
                    true
                );
                if(!r)
                {
                    SDL_LogError(
                        SDL_LOG_CATEGORY_APPLICATION,
                        "Failed to mount \"%s\" to \"%s\": %s",
                        name.c_str(),
                        i.c_str(),
                        PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())
                    );
                }
            }
        }
    }
    void Deinitialize()
    {
        // empty
    }

    ResourceManager::ResourceManager() = default;
    ResourceManager::~ResourceManager()
    {
        
    }

    ResourceManager& ResourceManager::Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    graphic::Texture& ResourceManager::AddTexture(
        const std::string& name,
        graphic::Texture&& tex
    ) {
        return m_tex.emplace(
            name,
            std::move(tex)
        ).first->second;
    }
    graphic::Texture* ResourceManager::GetTexturePtr(const std::string& name)
    {
        auto iter = m_tex.find(name);
        if(iter == m_tex.end())
            return nullptr;
        else
            return &iter->second;
    }
    std::map<std::string, graphic::Texture>& ResourceManager::GetTextures()
    {
        return m_tex;
    }
}
