// Author: HenryAWE
// License: The 3-clause BSD License

#include "shader.hpp"
#include <sstream>


namespace awe::graphic
{
    IShaderProgram::IShaderProgram(IRenderer& renderer)
        : Super(renderer) {}

    IShaderProgram::~IShaderProgram() noexcept = default;

    void IShaderProgram::AddShaderSrc(
        ShaderType type,
        std::unique_ptr<std::istream> is
    ) {
        m_src.emplace_back(std::pair(
            type,
            std::move(is)
        ));
        m_is_submitted = false;
    }
    void IShaderProgram::AddShaderSrc(
        ShaderType type,
        std::string src
    ) {
        auto ss = std::make_unique<std::stringstream>(std::move(src));
        AddShaderSrc(type, std::move(ss));
    }

    IShaderProgram::ShaderSrc& IShaderProgram::GetSrc(std::size_t idx)
    {
        return m_src[idx];
    }
    std::size_t IShaderProgram::ShaderSrcCount() noexcept
    {
        return m_src.size();
    }
    void IShaderProgram::ClearShaderSrc() noexcept
    {
        std::vector<ShaderSrc>().swap(m_src);
    }

    void IShaderProgram::DataSubmitted()
    {
        ClearShaderSrc();
        m_is_submitted = true;
    }
}
