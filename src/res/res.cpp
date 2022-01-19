// Author: HenryAWE
// License: The 3-clause BSD License

#include "res.hpp"
#include <filesystem>
#include <stb_image.h>


namespace awe::res
{
    namespace detailed
    {
        // WARNING: Special data is required by PhysFS on Android. Pay attention when porting to Android
        void InitPhysfs(const char* argv0)
        {
            if(PHYSFS_init(argv0) == 0)
            {
                throw std::runtime_error(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
            }

            PHYSFS_Version physfs_ver;
            PHYSFS_VERSION(&physfs_ver);
            // runtime version, may be different from the compile-time version
            PHYSFS_Version physfs_ver_rt;
            PHYSFS_getLinkedVersion(&physfs_ver_rt);
            SDL_LogInfo(
                SDL_LOG_CATEGORY_APPLICATION,
                "PhysicsFS version: %d.%d.%d\n"
                "PhysicsFS version (runtime): %d.%d.%d",
                physfs_ver.major, physfs_ver.major, physfs_ver.patch,
                physfs_ver_rt.major, physfs_ver_rt.major, physfs_ver_rt.patch
            );
        }
        void QuitPhysfs() noexcept
        {
            PHYSFS_deinit();
        }
    }

    void Initialize(const AppInitData& initdata)
    {
        // Setup stb image library
        stbi_set_flip_vertically_on_load(true);

        // Initialize virtual filesystem
        namespace fs = std::filesystem;
        detailed::InitPhysfs(initdata.argv[0]);
        PHYSFS_mount(
            (const char*)fs::current_path().u8string().c_str(),
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
        detailed::QuitPhysfs();
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
}
