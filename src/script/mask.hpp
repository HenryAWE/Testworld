// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SCRIPT_MASK_HPP
#define TESTWORLD_SCRIPT_MASK_HPP


namespace awe::script
{
    enum ModuleMask : int
    {
        MODULE_Default = 1,
        MODULE_Editor = 1 << 1,
        MODULE_Resource = 1 << 2
    };
}

#endif
