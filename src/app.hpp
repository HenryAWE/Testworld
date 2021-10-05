// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_APP_HPP
#define TESTWORLD_APP_HPP

#include <functional>
#include <memory>
#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <imgui.h>
#include "editor/editor.hpp"
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

        void PrepareScriptEnv();
        void ClearScriptEnv();

        void Quit();

        std::function<bool()> BeforeQuit;

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<Renderer> m_renderer;
        ImGuiContext* m_imgui_ctx = nullptr;

        std::unique_ptr<Editor> m_editor;

        asIScriptEngine* m_as_engine;
        std::unique_ptr<CScriptBuilder> m_as_builder;

        void MessageCallback(const asSMessageInfo* msg);
    };
}

#endif
