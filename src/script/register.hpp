// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SCRIPT_REGISTER_HPP
#define TESTWORLD_SCRIPT_REGISTER_HPP

#include <angelscript.h>
#include "../editor/editor.hpp"


namespace awe::script
{
    void RegisterEditor(asIScriptEngine* engine, Editor* editor);
    void RegisterObject(asIScriptEngine* engine);
}


#endif
