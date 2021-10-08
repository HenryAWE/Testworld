// Author: HenryAWE
// License: The 3-clause BSD License

#include "singleinstance.hpp"
#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN 1
#   include <Windows.h>
#   include <filesystem>
#endif
#include <SDL.h>


namespace awe
{
    namespace detailed
    {
#       ifdef _WIN32
        static HANDLE handle = nullptr;
        static bool Acquire()
        {
            auto filename = std::filesystem::temp_directory_path() / "testworld.lock";
            handle =  CreateFileA(
                filename.string().c_str(),
                GENERIC_READ,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            if(handle != INVALID_HANDLE_VALUE)
                return true;
            int err = GetLastError();
            switch(err)
            {
            case ERROR_SHARING_VIOLATION:
                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR,
                    "Sharing Violation",
                    "testworld.lock is acquired by other process, "
                    "you probably have already open a Testworld application",
                    nullptr
                );
                break;

            default:
                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR,
                    "Error",
                    "Could not acquire testworld.lock",
                    nullptr
                );
                break;
            }
            return false;
        }
        static void Release()
        {
            if(handle)
            {
                CloseHandle(handle);
                handle = NULL;
            }
        }
#       else
        bool Acquire()
        {
            return true;
        }
        void Release() {}
#       endif
    }

    SingleInstance::~SingleInstance()
    {
        detailed::Release();
    }

    SingleInstance& SingleInstance::GetInstance()
    {
        static SingleInstance instance;
        return instance;
    }

    bool SingleInstance::Acquire()
    {
        return detailed::Acquire();
    }
}
