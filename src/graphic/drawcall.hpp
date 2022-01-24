// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_DRAWCALL_HPP
#define TESTWORLD_GRAPHIC_DRAWCALL_HPP

#include <cstddef>
#include <variant>
#include <span>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "interface.hpp"


namespace awe::graphic
{
    class IMesh;
    class IShaderProgram;
    class ITexture2D;

    class ShaderUniformArg
    {
    public:
        typedef std::variant<
            std::int32_t,
            float,
            glm::vec3,
            glm::vec4,
            glm::mat4
        > ArgType;

        std::string name;
        ArgType arg;
    };
    class DynShaderUniformArg
    {
    public:
        typedef std::variant<
            std::int32_t*,
            float*,
            glm::vec3*,
            glm::vec4*,
            glm::mat4*
        > ArgType;

        std::string name;
        ArgType arg;
    };

    enum class Culling
    {
        None = 0,
        Front,
        Back
    };

    class DrawCallSettings
    {
    public:
        bool depth_test;
        bool mesh;
        bool blending;
        Culling culling;

        DrawCallSettings() noexcept;

        DrawCallSettings& operator=(const DrawCallSettings&) = default;
    };

    class IDrawCall : public InterfaceBase
    {
        typedef InterfaceBase Super;
    public:
        IDrawCall(IRenderer& renderer);

        virtual void Draw() = 0;

        void Submit();
        [[nodiscard]]
        constexpr bool IsSubmitted() const noexcept { return m_is_submitted; }

        [[nodiscard]]
        constexpr IMesh* GetMesh() noexcept { return m_mesh; }
        void SetMesh(IMesh* mesh);
        [[nodiscard]]
        constexpr IShaderProgram* GetShaderProgram() noexcept { return m_program; }
        void SetShaderProgram(IShaderProgram* program);
        [[nodiscard]]
        ITexture2D* GetTexture(std::size_t unit = 0) noexcept;
        [[nodiscard]]
        std::size_t GetTextureCount() const noexcept { return m_textures.size(); }
        void SetTexture(ITexture2D* texture, std::size_t unit = 0);
        [[nodiscard]]
        constexpr const DrawCallSettings& GetSettings() const noexcept { return m_settings; }
        void SetSettings(const DrawCallSettings& settings);
        [[nodiscard]]
        constexpr DrawCallSettings& ModifySettings() noexcept { return m_settings; }

        template <typename T>
        void AddUniformArg(std::string name, const T& arg)
        {
            m_args.emplace_back(std::move(name), arg);
        }

        template <typename T>
        void AddDynUniformArg(const std::string& name, const T* ptr)
        {
            m_dynarg.emplace_back(std::move(name), ptr);
        }

    protected:
        virtual void UpdateData(
            IMesh& mesh,
            std::span<ITexture2D*> textures,
            IShaderProgram& program,
            std::span<ShaderUniformArg> args,
            const DrawCallSettings& settings
        ) = 0;

        void DataSubmitted();

    private:
        IMesh* m_mesh = nullptr;
        std::vector<ITexture2D*> m_textures;
        IShaderProgram* m_program;
        std::vector<ShaderUniformArg> m_args;
        std::vector<DynShaderUniformArg> m_dynarg;
        DrawCallSettings m_settings;

        bool m_is_submitted = false;
    };
}


#endif
