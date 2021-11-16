// Author: HenryAWE
// License: The 3-clause BSD License

#include <glad/glad.h>
#include "renderer.hpp"
#include <cassert>
#include <stdexcept>
#include <vector>
#include <fmt/format.h>
#include "../window/window.hpp"


namespace awe
{
    Renderer::Renderer(Window& window)
        : m_window(window) {}

    Renderer::~Renderer() noexcept
    {
        if(m_context)
            DestroyContext();
    }

    void Renderer::CreateContext()
    {
        assert(!m_context);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        m_context = SDL_GL_CreateContext(m_window.GetHandle());
        if(!m_context)
        {
            throw std::runtime_error(SDL_GetError());
        }
        if(!gladLoadGLLoader(&SDL_GL_GetProcAddress))
        {
            SDL_GL_DeleteContext(m_context);
            m_context = nullptr;
            throw std::runtime_error("gladLoadGLLoader() failed");
        }

        InitData();
    }
    void Renderer::DestroyContext() noexcept
    {
        ReleaseData();
        assert(m_context);
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
    }

    void Renderer::Present()
    {
        SDL_GL_SwapWindow(m_window.GetHandle());
    }

    SDL_GLContext Renderer::GetContext() const noexcept
    {
        return m_context;
    }

    glm::ivec2 Renderer::GetDrawableSize() const
    {
        glm::ivec2 size;
        SDL_GL_GetDrawableSize(m_window.GetHandle(), &size[0], &size[1]);
        return size;
    }
    std::string Renderer::RendererInfo()
    {
        return fmt::format(
            "OpenGL Information\n"
            "Vendor: {}\n"
            "Renderer: {}\n"
            "Version: {}",
            glGetString(GL_VENDOR),
            glGetString(GL_RENDERER),
            glGetString(GL_VERSION)
        );
    }

    Texture Renderer::GeneratePerlinTexture2D(float p, int size)
    {
        auto Fill = [p](int x, int y)->unsigned
        {
            float val = graphic::PerlinNoise<float>(p, (float)x, (float)y);
            return unsigned char(256 * val);
        };
        std::vector<unsigned char> buf;
        buf.resize(size * size);
        for(int i = 0; i < size; ++i)
        {
            for(int j = 0; j < size; ++j)
            {
                buf[i * size + j] = Fill(i, j);
            }
        }
        Texture tex;
        TexDescription desc;
        desc.format = GL_RED;
        desc.type = GL_RED;
        tex.LoadMemoryEx(
            buf.data(),
            glm::ivec2(size),
            true,
            desc
        );
        assert(glGetError() == GL_NO_ERROR);

        return std::move(tex);
    }

    Framebuffer& Renderer::GetFramebuffer()
    {
        return m_fbo;
    }
    Texture& Renderer::GetScreenTexture()
    {
        return m_screen_tex;
    }
    void Renderer::DrawTexture(
        const Texture& tex,
        const glm::mat4& matrix,
        bool custom_shader
    ) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        if(!custom_shader) glUseProgram(m_rect_shader);
        glBindVertexArray(m_rect_vao);

        GLint uniloc = m_rect_shader.UniLoc("matrix");
        if(uniloc != -1) Uniform(uniloc, matrix);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        if(!custom_shader) glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Renderer::InitData()
    {
        const auto size = GetDrawableSize();

        m_fbo.Generate();
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        m_screen_tex.Generate();
        m_fbo.Generate();
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        TexDescription screnn_desc{};
        screnn_desc.format = GL_RGB;
        m_screen_tex.LoadFramebuffer(
            size,
            screnn_desc,
            GL_COLOR_ATTACHMENT0,
            GL_RGB
        );

        m_rbo.Generate();
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            size[0],
            size[1]
        );
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            m_rbo
        );
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Incomplete OpenGL frame buffer"
            );
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // rectangle data for drawing texture

        const float vertices[] =
        {
            1.0f, 1.0f,     1.0f, 1.0f,
            1.0f, -1.0f,    1.0f, 0.0f,
            -1.0f, -1.0f,   0.0f, 0.0f,
            -1.0f, 1.0f,    0.0f, 1.0f
        };

        const unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        m_rect_shader.Generate();
        const char* rect_vs =
            "#version 330 core\n"
            "layout (location=0) in vec2 pos;\n"
            "layout (location=1) in vec2 in_coord;\n"
            "out vec2 coord;\n"
            "uniform mat4 matrix;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = matrix * vec4(pos, 0.0f, 1.0f);\n"
                "coord = in_coord;\n"
            "}";
        const char* rect_fs =
            "#version 330 core\n"
            "in vec2 coord;\n"
            "out vec4 frag;\n"
            "uniform sampler2D tex;\n"
            "void main()\n"
            "{\n"
            "   frag = texture(tex, coord);\n"
            "}";
        bool shader_status = m_rect_shader.Compile(rect_vs, rect_fs);
        assert(shader_status);

        m_rect_vao.Generate();
        m_rect_vbo.Generate();
        m_rect_ebo.Generate();

        glBindVertexArray(m_rect_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_rect_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rect_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    void Renderer::ReleaseData()
    {
        m_rect_vao.Destroy();
        m_rect_ebo.Destroy();
        m_rect_vbo.Destroy();
        m_rect_shader.Destroy();
    }
}
