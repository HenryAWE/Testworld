// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_EDITOR_PIXELPAINTER_HPP
#define TESTWORLD_EDITOR_PIXELPAINTER_HPP

#include <functional>
#include <vector>
#include <tuple>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>


namespace awe
{
    class PixelPainter
    {
    public:
        PixelPainter();

        void NewFrame();

        glm::u8vec4 GetColor(glm::ivec2 coord);

    private:
        /* Children */
        void MenuBar();
        void Toolbox();
        void Canvas();
        void BottomToolbox();

        void MenuBar_File();

        void NewBitmap(const std::string& name, glm::ivec2 size);

        std::function<void()> m_popup;

        bool m_open = true;
        glm::vec4 m_color[2];
        int m_current_tool_id = 0;
        float m_factor = 1.0f;
        /* Bitmap data */
        std::vector<glm::u8vec4> m_bitmap; //RGBA
        // The total bitmap size will be square of this variable
        std::size_t m_side = 0;
    };
}

#endif
