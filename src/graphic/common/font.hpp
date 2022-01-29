// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_COMMON_FONT_HPP
#define TESTWORLD_GRAPHIC_COMMON_FONT_HPP

#include <istream>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "image.hpp"


namespace awe::graphic::common
{
    class FontBuilder;

    class GlyphLookupTable
    {
        friend class FontBuilder;
    public:
        struct GlyphData
        {
            glm::uvec2 coord;
            glm::uvec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
        };

        typedef boost::container::flat_map<char32_t, GlyphData> Container;

        GlyphLookupTable() = default;
        GlyphLookupTable(GlyphLookupTable&&) = default;

        GlyphLookupTable& operator=(GlyphLookupTable&&) = default;

        Container::iterator begin() noexcept { return m_data.begin(); }
        Container::iterator end() noexcept { return m_data.end(); }

        Container::iterator find(char32_t cp) { return m_data.find(cp); }

        [[nodiscard]]
        Container& GetContainer() noexcept { return m_data; }

    private:
        Container m_data;
    };

    class FontBuilder
    {
    public:
        FontBuilder(FT_Library ftlib);

        ~FontBuilder() noexcept;

        bool LoadFontVfs(
            const std::string& file,
            glm::uvec2 size = glm::uvec2(0, 48)
        );

        void Release() noexcept;

        std::pair<Image2D<1>, GlyphLookupTable> GenerateAtlas(char32_t begin, char32_t end);

        [[nodiscard]]
        constexpr FT_Library GetLibrary() const noexcept { return m_ftlib; }

    private:
        FT_Library m_ftlib;
        FT_Face m_face;
        std::vector<std::byte> m_buffer;
    };
}


#endif
