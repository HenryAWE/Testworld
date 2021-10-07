// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_UI_CONSOLE_HPP
#define TESTWORLD_UI_CONSOLE_HPP

#include <deque>
#include <string>
#include <imgui.h>
#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>


namespace awe::imgui
{
    class Console
    {
    public:
        void NewFrame();

        void Write(std::string_view sv);

        int SetScriptEngine(asIScriptEngine* engine, CScriptBuilder* builder);
        void ReleaseScriptEngine();

    private:
        bool m_show = true;
        bool m_fixed_mode = true; // false for windowed mode

        char m_input_buf[512];

        void OutputRegion();

        typedef std::string ConsoleOutputLine;
        std::deque<ConsoleOutputLine> m_outputs;

        void DrawLine(const ConsoleOutputLine& line);

        asIScriptEngine* m_as_engine;
        asIScriptContext* m_console_ctx;
        asIScriptModule* m_mod;
    };
}


#endif
