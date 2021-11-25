// Author: HenryAWE
// License: The 3-clause BSD License

#include "register.hpp"
#include "callconv.hpp"
#include "../editor/editor.hpp"


namespace awe::script
{
    [[noreturn]]
    void ThrowErr(int r)
    {
        throw std::runtime_error("Angelscript error: " + std::to_string(r));
    }

#   define CHECK_R(r) if(r < 0) { ThrowErr(r); }
    void RegisterEditor(asIScriptEngine* engine, Editor* editor)
    {
        int r = 0;
        r = engine->RegisterObjectType("Editor", sizeof(Editor), asOBJ_REF | asOBJ_NOCOUNT);
        CHECK_R(r);
        r = engine->RegisterObjectMethod(
            "Editor", "void ShowInfo()",
            asMETHOD(Editor, ShowInfo), asCALL_THISCALL
        );
        CHECK_R(r);
        r = engine->RegisterGlobalProperty("Editor editor", editor);
        CHECK_R(r);
    }
}
