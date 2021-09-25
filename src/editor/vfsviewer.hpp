// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_EDITOR_VFSVIEWER_HPP
#define TESTWORLD_EDITOR_VFSVIEWER_HPP

#include <memory>
#include <string>
#include <imgui.h>


namespace awe
{
    namespace detailed
    {
        struct CacheNode;
    }

    class VfsViewer
    {
    public:
        VfsViewer();

        void NewFrame();

        void Show() noexcept;
        void Hide() noexcept;
        [[nodiscard]]
        constexpr bool IsShow() const noexcept { return m_open; }

    private:
        bool m_open = true;
        std::shared_ptr<detailed::CacheNode> m_root;

        void BuildNode();
    };
}

#endif
