// Author: HenryAWE
// License: The 3-clause BSD License

#include "singleinstance.hpp"
#include <SDL.h>
#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN 1
#   include <Windows.h>
#   include <filesystem>
#elif defined(__LINUX__)
#   include <sys/file.h>
#   include <unistd.h>
#   include <errno.h>
#endif
#include <fmt/core.h>


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
#       elif defined(__LINUX__)
        static int pid_file = 0;
        static int lock = 0;
        bool Acquire()
        {
            #define TW_PIDFILE_PATH "/tmp/testworld.pid"
            pid_file = open(TW_PIDFILE_PATH, O_CREAT | O_RDWR, 0666);
            if(pid_file == -1)
            {
                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR,
                    "Error",
                    fmt::format(
                        "Failed to open " TW_PIDFILE_PATH " : {}",
                        strerror(errno)
                    ).c_str(),
                    nullptr
                );

                return false;
            }
            lock = flock(pid_file, LOCK_EX | LOCK_NB);
            if(lock)
            {
                if(errno == EWOULDBLOCK)
                {
                    SDL_ShowSimpleMessageBox(
                        SDL_MESSAGEBOX_ERROR,
                        "EWOULDBLOCK",
                        TW_PIDFILE_PATH " is acquired by other process, "
                        "you probably have already open a Testworld application",
                        nullptr
                    );

                    return false;
                }

                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR,
                    "Error",
                    "Could not acquire " TW_PIDFILE_PATH,
                    nullptr
                );

                return false;
            }

            return true;
        }
        void Release()
        {
            flock(lock, LOCK_UN);
            lock = 0;
            close(pid_file);
            pid_file = 0;
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
