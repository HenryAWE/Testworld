// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SCRIPT_MASK_HPP
#define TESTWORLD_SCRIPT_MASK_HPP


namespace awe::script
{
    enum ModuleMask : int
    {
        Default = 1,
        Editor = 1 << 1,
        Resource = 1 << 2
    };
}

#endif
