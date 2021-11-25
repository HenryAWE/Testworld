// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RES_RES_HPP
#define TESTWORLD_RES_RES_HPP

#include <map>
#include <optional>
#include "../graphic/renderer.hpp"
#include "vfs.hpp"


namespace awe::res
{
    void Initialize();
    void Deinitialize();

    class ResourceManager
    {
    public:
        ResourceManager();
        ~ResourceManager();

        static ResourceManager& Instance();

        Texture& AddTexture(
            const std::string& name,
            Texture&& tex
        );
        /* Will return nullptr when required texture cannot be found */
        Texture* GetTexturePtr(const std::string& name);
        std::map<std::string, Texture>& GetTextures();

    private:
        std::map<std::string, Texture> m_tex;
    };
}

#endif
