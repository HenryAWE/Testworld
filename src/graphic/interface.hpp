// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_INTERFACE_HPP
#define TESTWORLD_GRAPHIC_INTERFACE_HPP

namespace awe::graphic
{
    class IRenderer;

    class InterfaceBase
    {
    public:
        InterfaceBase(IRenderer& renderer) noexcept;
        InterfaceBase(const InterfaceBase&) = delete;

        virtual ~InterfaceBase() noexcept;

        [[nodiscard]]
        constexpr IRenderer& GetRenderer() noexcept { return m_renderer; }

    private:
        IRenderer& m_renderer;
    };
}


#endif
