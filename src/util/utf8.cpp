// Author: HenryAWE
// License: The 3-clause BSD License

#include "utf8.hpp"
#include <utf8.h>


namespace awe::util
{
    char32_t ConstUtf8Iterator::Next()
    {
        utf8_int32_t cp;
        m_ptr = utf8codepoint(m_ptr, &cp);
        return cp;
    }
    char32_t ConstUtf8Iterator::Current()
    {
        utf8_int32_t cp;
        utf8codepoint(m_ptr, &cp);
        return cp;
    }
}
