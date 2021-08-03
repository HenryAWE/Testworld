// Author: HenryAWE
// License: The 3-clause BSD License

#include "window.hpp"
#include <cassert>
#include <stdexcept>


namespace awe
{
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
