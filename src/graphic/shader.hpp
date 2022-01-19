// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_SHADER_HPP
#define TESTWORLD_GRAPHIC_SHADER_HPP

#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>
#include "interface.hpp"


namespace awe::graphic
{
    class IRenderer;

    enum class ShaderType : int
    {
        VERTEX = 1,
        FRAGMENT,
    };

    class IShaderProgram : public InterfaceBase
    {
        typedef InterfaceBase Super;
    public:
        IShaderProgram(IRenderer& renderer);

        ~IShaderProgram() noexcept;

        void AddShaderSrc(
            ShaderType type,
            std::unique_ptr<std::istream> is
        );
        void AddShaderSrc(
            ShaderType type,
            std::string src
        );
        template <typename T>
        std::enable_if_t<std::is_base_of_v<std::istream, T>> AddShaderSrc(
            ShaderType type,
            std::unique_ptr<T> is
        ) {
            AddShaderSrc(
                type,
                std::unique_ptr<std::istream>(is.release())
            );
        }

        // Thread safety: Can only be called in rendering thread
        virtual void Submit() = 0;

        [[nodiscard]]
        constexpr bool IsSubmitted() noexcept
        {
            return m_is_submitted || m_src.empty();
        }

    protected:
        typedef std::pair<ShaderType, std::unique_ptr<std::istream>> ShaderSrc;

        ShaderSrc& GetSrc(std::size_t idx);
        std::size_t ShaderSrcCount() noexcept;
        void ClearShaderSrc() noexcept;

        void DataSubmitted();

    private:
        std::vector<ShaderSrc> m_src;
        bool m_is_submitted = false;
    };
}


#endif
