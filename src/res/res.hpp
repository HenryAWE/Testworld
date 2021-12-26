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
    };
}

#endif
