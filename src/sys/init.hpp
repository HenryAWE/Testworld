// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SYS_INIT_HPP
#define TESTWORLD_SYS_INIT_HPP

namespace awe
{
    struct AppInitData
    {
        int argc = 0;
        char** argv = nullptr;
        bool ogl_debug = false;

        AppInitData(
            int argc_, char* argv_[],
            bool ogl_debug_
        ) noexcept :
        argc(argc_), argv(argv_),
        ogl_debug(ogl_debug_) {}
    };

    void Prepare(const char* argv0);
}

#endif
