// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_OPENGL3_RENDERER_HPP
#define TESTWORLD_GRAPHIC_OPENGL3_RENDERER_HPP

#include <glad/glad.h>
#include "../renderer.hpp"
#include <atomic>
#include <queue>
#include "../../sys/init.hpp"
#include "glutil.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "drawcall.hpp"


namespace awe::graphic::opengl3
{
    namespace detailed
    {
        class Task
        {
        public:
            virtual void Invoke() = 0;
        };
    }

    /* OpenGL 3 Renderer */
    class Renderer : public graphic::IRenderer
    {
        typedef IRenderer Super;
    public:
        Renderer(
            window::Window& window,
            const AppInitData& initdata
        );

        ~Renderer() noexcept;

        virtual bool Initialize() override;
        virtual void Deinitialize() noexcept override;
        virtual bool IsInitialized() noexcept override;

        std::string GetRendererName() override;

        [[nodiscard]]
        constexpr SDL_GLContext GetContext() const noexcept
        {
            return m_context;
        }

        // Return true to ignore the current debug output message
        std::function<bool(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity
        )> DebugOutputFilter;

        void BeginMainloop() override;
        void QuitMainloop() override;

        void Present() override;

        std::future<std::string> QueryRendererInfo() override;

        glm::ivec2 GetDrawableSize() const override;
        bool IsRuntimeShaderCompilationSupported() const override;
        bool IsDataTransposeSupported() const override;

        // Resources generator
        std::unique_ptr<Mesh> CreateMesh(bool dynamic = false);
        std::unique_ptr<ShaderProgram> CreateShaderProgram();
        std::unique_ptr<Texture2D> CreateTexture2D();
        std::unique_ptr<DrawCall> CreateDrawCall();

        void PushClearCommand(std::function<void()> func);

    protected:
        Mesh* NewMesh(bool dynamic) override;
        ShaderProgram* NewShaderProgram() override;
        Texture2D* NewTexture2D() override;
        DrawCall* NewDrawCall() override;

        void NewData() override;
        void DeleteData() noexcept override;

    private:
        bool m_initialized = false;

        void CreateContext(bool debug = false);
        void DestroyContext() noexcept;

        // ImGui helpers
        void InitImGuiImpl();
        void ShutdownImGuiImpl();
        void ImGuiImplRenderDrawData();

        std::string RendererInfo();

        SDL_GLContext m_context = nullptr;
        bool m_debug = false;

        // Thread safety: Can only be called in the main thread
        bool DetachThread();
        // Thread safety: Can only be called in the rendering thread
        void RendererMain();
        // Thread safety: Can be called in any thread
        void QuitRenderThread();

        std::thread::id m_render_thread_id;
        std::atomic_bool m_request_render = false;
        std::atomic_bool m_presented = false;
        std::atomic_bool m_quit_mainloop = false;
        std::atomic_bool m_begin_mainloop = false;
        std::atomic_bool m_is_data_released = true;

        void AttachDebugCallback();
        bool DefaultDebugOutputFilter(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity
        );
        void DebugOutput(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity, 
            std::string_view message
        );

        void ExecuteClearCommand();
        void ExecuteQueryCommand();

        void PushQueryCommand(std::unique_ptr<detailed::Task>&& task);

        std::mutex m_clear_cmd_mutex;
        std::queue<std::function<void()>> m_clear_cmd;
        std::mutex m_query_cmd_mutex;
        std::queue<std::unique_ptr<detailed::Task>> m_query_cmd;
    };

    class OpenGL3ImGuiRenderer : public IImGuiRenderer
    {
        typedef IImGuiRenderer Super;
    public:
        OpenGL3ImGuiRenderer(Renderer& renderer);

        ~OpenGL3ImGuiRenderer() noexcept;

        // Thread safety: Can only be called in rendering thread
        void CreateDeviceObjects() override;
        // Thread safety: Can only be called in rendering thread
        void DestroyDeviceObjects() noexcept override;

        void Draw(ImDrawData* drawdata) override;

        [[nodiscard]]
        const std::string& GetName() override;
        [[nodiscard]]
        Renderer& GetRenderer() noexcept;

        [[nodiscard]]
        ImTextureID ToImTextureId(const ITexture2D& tex) noexcept override;
    };
}


#endif
