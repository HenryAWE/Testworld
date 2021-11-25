// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SCRIPT_SCRIPT_HPP
#define TESTWORLD_SCRIPT_SCRIPT_HPP

#include <angelscript.h>
#include "callconv.hpp"
#include "mask.hpp"
#include "register.hpp"
#include "scriptutil.hpp"


namespace awe::script
{
    int InitScriptEnv(asIScriptEngine* engine);
    void ClearScriptEnv(asIScriptEngine* engine);
}

#endif
