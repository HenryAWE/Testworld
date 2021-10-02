// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SCRIPT_CALLCONV_HPP
#define TESTWORLD_SCRIPT_CALLCONV_HPP

#ifdef _MSV_VER
#   define TW_CDECL __cdecl
#elif defined(__GNUC__)
#   define TW_CDECL __attribute__((__cdecl__))
#else
#   warning "MacroTW_CDECL is not explicitly defined, may causing unwanted behavior"
#   define TW_CDECL
#endif

#endif
