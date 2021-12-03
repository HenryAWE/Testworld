// Author: HenryAWE
// License: The 3-clause BSD License

#include "init.hpp"
#include <SDL.h>
#include <filesystem>
#include <stdexcept>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif
#include <physfs.h>


namespace awe
{
    void Prepare(const char* argv0)
    {
#ifdef _WIN32
        (void)argv0;

        wchar_t buf[MAX_PATH];
        ::GetModuleFileNameW(nullptr, buf, MAX_PATH);

        using namespace std::filesystem;
        std::error_code ec;
        current_path(path(buf).parent_path(), ec);
        if(ec)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Set program execution path failed: %s",
                ec.message().c_str()
            );
        }
#elif defined(__LINUX__)
        std::error_code ec;
        current_path(std::filesystem::path(argv0).parent_path(), ec);
        if(ec)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Set program execution path failed: %s",
                ec.message().c_str()
            );
        }
#else
#error "Unimplemented OS-depended code"
#endif
    }
}
