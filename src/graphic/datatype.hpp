// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_DATATYPE_HPP
#define TESTWORLD_GRAPHIC_DATATYPE_HPP

#include <cstddef>


namespace awe::graphic
{
    enum class DataType : int
    {
        FLOAT = 1,
        BYTE,
        UBYTE,
        SHORT,
        USHORT,
        INT,
        UINT
    };

    template <typename T>
    DataType GetDataType() = delete;
    template <>
    constexpr DataType GetDataType<float>() noexcept { return DataType::FLOAT; }
    template <>
    constexpr DataType GetDataType<signed char>() noexcept { return DataType::BYTE; }
    template <>
    constexpr DataType GetDataType<unsigned char>() noexcept { return DataType::UBYTE; }
    template <>
    constexpr DataType GetDataType<short>() noexcept { return DataType::SHORT; }
    template <>
    constexpr DataType GetDataType<unsigned short>() noexcept { return DataType::USHORT; }
    template <>
    constexpr DataType GetDataType<int>() noexcept { return DataType::INT; }
    template <>
    constexpr DataType GetDataType<unsigned int>() noexcept { return DataType::UINT; }

    std::size_t SizeOf(DataType type) noexcept;
}


#endif
