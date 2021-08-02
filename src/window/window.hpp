// Author: HenryAWE
// License: The 3-clause BSD License

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
