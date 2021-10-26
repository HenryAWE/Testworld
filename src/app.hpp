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
#include "lang/lang.hpp"
#include "renderer/renderer.hpp"
#include "ui/console.hpp"
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

        void LoadLanguagePak(const std::string& pakname = "en-US");

        void Mainloop();

        void PrepareScriptEnv();
        void ClearScriptEnv();

        void Quit();

        LangPak& GetLanguagePak();

        std::function<bool()> BeforeQuit;

    private:
        std::shared_ptr<Window> m_window;
        std::shared_ptr<Renderer> m_renderer;
        ImGuiContext* m_imgui_ctx = nullptr;

        LangPak m_lang;

        std::unique_ptr<Editor> m_editor;

        asIScriptEngine* m_as_engine;
        std::unique_ptr<CScriptBuilder> m_as_builder;

        void MessageCallback(const asSMessageInfo* msg);

        std::unique_ptr<imgui::Console> m_console;
    };
}

#endif
