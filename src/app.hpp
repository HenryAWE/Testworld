// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_APP_HPP
#define TESTWORLD_APP_HPP

#include <functional>
#include <memory>
#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <imgui.h>
#include "sys/init.hpp"
#include "editor/editor.hpp"
#include "res/lang/lang.hpp"
#include "graphic/renderer.hpp"
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

        void Initialize(const AppInitData& initdata);
        void Deinitialize();

        void LoadLanguagePak(const std::string& pakname = "en-US");

        void Mainloop();



        LangPak& GetLanguagePak();

        std::function<bool()> BeforeQuit;

    private:
        void PrepareScriptEnv();
        void ClearScriptEnv();

        std::shared_ptr<window::Window> m_window;
        std::shared_ptr<graphic::IRenderer> m_renderer;
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
