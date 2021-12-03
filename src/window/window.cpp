// Author: HenryAWE
// License: The 3-clause BSD License

#include "window.hpp"
#include <cassert>
#include <stdexcept>


namespace awe::window
{
    namespace detailed
    {
        void InitSDL()
        {
            constexpr int required_subsystems =
                SDL_INIT_AUDIO |
                SDL_INIT_VIDEO;

            if(SDL_Init(required_subsystems) < 0)
            {
                throw std::runtime_error(SDL_GetError());
            }

            SDL_version sdl_ver;
            SDL_VERSION(&sdl_ver);
            // runtime version, may be different from the compile-time version
            SDL_version sdl_ver_rt;
            SDL_GetVersion(&sdl_ver_rt);
            SDL_LogInfo(
                SDL_LOG_CATEGORY_APPLICATION,
                "SDL version: %d.%d.%d\n"
                "SDL version (runtime): %d.%d.%d",
                sdl_ver.major, sdl_ver.major, sdl_ver.patch,
                sdl_ver_rt.major, sdl_ver_rt.major, sdl_ver_rt.patch
            );
        }
        void QuitSDL() noexcept
        {
            SDL_Quit();
        }
    }
        
    void Initialize(const AppInitData& initdata)
    {
        detailed::InitSDL();
    }
    void Deinitialize() noexcept
    {
        detailed::QuitSDL();
    }

    Window::Window(
        const std::string& title,
        int width,
        int height,
        int additional_flags
    ) {
        CreateWindow(
            title,
            width,
            height,
            additional_flags
        );
    }

    Window::~Window() noexcept
    {
        DestroyWindow();
    }

    glm::ivec2 Window::GetSize() const
    {
        glm::ivec2 size;
        SDL_GetWindowSize(m_handle, &size[0], &size[1]);
        return size;
    }

    SDL_Window* Window::GetHandle() const noexcept
    {
        return m_handle;
    }

    void Window::CreateWindow(
        const std::string& title,
        int width,
        int height,
        int additional_flags
    ){
        assert(!m_handle);

        const int flags = additional_flags |
            SDL_WINDOW_OPENGL;
        m_handle = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            flags
        );
        if(!m_handle)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void Window::DestroyWindow() noexcept
    {
        SDL_DestroyWindow(m_handle);
        m_handle = nullptr;
    }
}
