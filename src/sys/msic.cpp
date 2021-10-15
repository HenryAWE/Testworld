// Author: HenryAWE
// License: The 3-clause BSD License

#include "misc.hpp"
#include <SDL_platform.h>
#ifdef __WINDOWS__
#   define WIN32_LEAN_AND_MEAN 1
#   include <Windows.h>
#   include <shellapi.h>
#endif


namespace awe
{
#ifdef __WINDOWS__
    void OpenUrl(const std::string& url)
    {
        int wsize = MultiByteToWideChar(
            CP_UTF8,
            0,
            url.c_str(),
            (int)url.length(),
            nullptr,
            0
        );
        if(wsize <= 0)
            return;
        std::wstring wurl;
        wurl.resize(wsize);
        MultiByteToWideChar(
            CP_UTF8,
            0,
            url.c_str(),
            (int)url.length(),
            wurl.data(),
            (int)wurl.length()
        );

        ShellExecuteW(
            NULL,
            L"open",
            wurl.c_str(),
            NULL,
            NULL,
            SW_SHOWNORMAL
        );
    }
#else
    void OpenUrl(const std::string& url) {}
#endif
}
