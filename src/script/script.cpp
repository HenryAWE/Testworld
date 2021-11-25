// Author: HenryAWE
// License: The 3-clause BSD License

#include "script.hpp"
#include <scriptarray/scriptarray.h>
#include <scriptstdstring/scriptstdstring.h>


namespace awe::script
{
    int InitScriptEnv(asIScriptEngine* engine)
    {
        int r = 0;

        RegisterScriptArray(engine, true);
        RegisterStdString(engine);
        RegisterStdStringUtils(engine);

        return r;
    }
    void ClearScriptEnv(asIScriptEngine* engine)
    {
        // empty
        (void)engine;
    }
}
