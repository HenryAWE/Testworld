// Author: HenryAWE
// License: The 3-clause BSD License

#include "res.hpp"



namespace awe
{
    ResourceManager::ResourceManager() = default;
    ResourceManager::~ResourceManager()
    {
        
    }

    ResourceManager& ResourceManager::Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    Texture& ResourceManager::AddTexture(
        const std::string& name,
        Texture&& tex
    ) {
        return m_tex.emplace(
            name,
            std::move(tex)
        ).first->second;
    }
    Texture* ResourceManager::GetTexturePtr(const std::string& name)
    {
        auto iter = m_tex.find(name);
        if(iter == m_tex.end())
            return nullptr;
        else
            return &iter->second;
    }
    std::map<std::string, Texture>& ResourceManager::GetTextures()
    {
        return m_tex;
    }
}
