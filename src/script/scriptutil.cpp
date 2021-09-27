// Author: HenryAWE
// License: The 3-clause BSD License

#include "scriptutil.hpp"
#include "../res/vfs.hpp"


namespace awe::script
{
    int AddSectionFromVfs(
        CScriptBuilder* builder,
        const std::string& filename
    ) {
        auto data = vfs::GetData(filename);
        return builder->AddSectionFromMemory(
            filename.c_str(),
            (const char*)data.data(),
            (unsigned int)data.size()
        );
    }
}