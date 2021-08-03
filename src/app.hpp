// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_APP_HPP
#define TESTWORLD_APP_HPP

#include <memory>
#include <imgui.h>
#include "renderer/renderer.hpp"
#include "window/window.hpp"


namespace awe
{
    class App
    {
        App();
    public:
        ~App();

        static App& GetInstance();

        void CreateWindow();

        void Mainloop();

        void Quit();

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<Renderer> m_renderer;
        ImGuiContext* m_imgui_ctx = nullptr;
    };
}

#endif
