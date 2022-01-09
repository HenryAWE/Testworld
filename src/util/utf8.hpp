// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_UTIL_UTF8_HPP
#define TESTWORLD_UTIL_UTF8_HPP


namespace awe::util
{
    class ConstUtf8Iterator
    {
        char* m_ptr = nullptr;
    public:
        ConstUtf8Iterator(char* u8str) noexcept : m_ptr(u8str) {}

        // Iterate to next codepoint and return the current one
        char32_t Next();

        char32_t Current();

        [[nodiscard]]
        constexpr bool operator!=(const ConstUtf8Iterator& rhs) noexcept
        {
            return m_ptr != rhs.m_ptr;
        }
        ConstUtf8Iterator& operator++() noexcept
        {
            Next();
            return *this;
        }
        char32_t operator*() noexcept
        {
            return Current();
        }
    };
}

#endif
