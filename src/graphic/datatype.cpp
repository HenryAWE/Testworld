// Author: HenryAWE
// License: The 3-clause BSD License

#include "datatype.hpp"
#include <cassert>
#include <cstdint>


namespace awe::graphic
{
    std::size_t SizeOf(DataType type) noexcept
    {
        switch(type)
        {
        case DataType::FLOAT: return sizeof(float);
        case DataType::BYTE: return sizeof(int8_t);
        case DataType::UBYTE: return sizeof(uint8_t);
        case DataType::SHORT: return sizeof(uint16_t);
        case DataType::USHORT: return sizeof(uint16_t);
        case DataType::INT: return sizeof(int32_t);
        case DataType::UINT: return sizeof(uint32_t);
        default: assert(false); return 0;
        }
    }
}
