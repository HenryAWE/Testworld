// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_SCRIPT_CALLCONV_HPP
#define TESTWORLD_SCRIPT_CALLCONV_HPP

#ifdef _MSC_VER
#   define TW_CDECL __cdecl
#elif defined(__GNUC__)
#   define TW_CDECL __attribute__((__cdecl__))
#   pragma GCC diagnostic ignored "-Wattributes"
#else
// WARNING: Macro TW_CDECL is not explicitly defined, may causing unwanted behavior
#   define TW_CDECL
#endif

#endif
