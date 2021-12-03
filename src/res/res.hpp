// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RES_RES_HPP
#define TESTWORLD_RES_RES_HPP

#include <map>
#include <optional>
#include "../sys/init.hpp"
#include "../graphic/renderer.hpp"
#include "vfs.hpp"


namespace awe::res
{
    void Initialize(const AppInitData& initdata);
    void Deinitialize();

    class ResourceManager
    {
    public:
        ResourceManager();
        ~ResourceManager();

        static ResourceManager& Instance();

        graphic::Texture& AddTexture(
            const std::string& name,
            graphic::Texture&& tex
        );
        /* Will return nullptr when required texture cannot be found */
        graphic::Texture* GetTexturePtr(const std::string& name);
        std::map<std::string, graphic::Texture>& GetTextures();

    private:
        std::map<std::string, graphic::Texture> m_tex;
    };
}

#endif
