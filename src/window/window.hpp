// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_WINDOW_WINDOW_HPP
#define TESTWORLD_WINDOW_WINDOW_HPP

#include <SDL.h>
#include <string>


namespace awe
{
    class Window
    {
    public:
        Window(
            const std::string& title,
            int width = 1280,
            int height = 960,
            int additional_flags = 0
        );

        ~Window() noexcept;

        SDL_Window* GetHandle() const noexcept;

    private:
        void CreateWindow(
            const std::string& title,
            int width,
            int height,
            int additional_flags
        );

        void DestroyWindow() noexcept;

        SDL_Window* m_handle = nullptr;
    };
}

#endif
