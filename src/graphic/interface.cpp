// Author: HenryAWE
// License: The 3-clause BSD License

#include "interface.hpp"


namespace awe::graphic
{
    InterfaceBase::InterfaceBase(IRenderer& renderer) noexcept
        : m_renderer(renderer) {}

    InterfaceBase::~InterfaceBase() noexcept = default;
}
