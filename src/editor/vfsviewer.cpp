// Author: HenryAWE
// License: The 3-clause BSD License

#include "vfsviewer.hpp"
#include <ctime>
#include <optional>
#include <vector>
#include <physfs.h>
#include <fmt/core.h>


namespace awe
{
    namespace detailed
    {
        struct CacheNode
        {
            CacheNode() {}
            CacheNode(const CacheNode&) = default;
            CacheNode(CacheNode&& move)
                : name(move.name),
                fullpath(move.fullpath),
                children(std::move(move.children)),
                info(std::move(move.info))
            {
                std::memcpy(&stat, &move.stat, sizeof(stat));
                std::memset(&move.stat, 0, sizeof(stat));
            }

            CacheNode& operator=(CacheNode&&) = default;

            std::string name;
            std::string fullpath;
            PHYSFS_Stat stat;

            std::vector<CacheNode> children;

            std::optional<std::string> info;

            void BuildChildren(const char* const* files)
            {
                for(int i = 0; files[i]; ++i)
                {
                    CacheNode child;
                    child.name = files[i];
                    child.fullpath.reserve(
                        fullpath.size() +
                        1 + child.name.size()
                    );
                    child.fullpath = fullpath;
                    if(name != "/" && fullpath != "")
                        child.fullpath.push_back('/');
                    child.fullpath.append(
                        child.name
                    );
                    PHYSFS_stat(
                        child.fullpath.c_str(),
                        &child.stat
                    );

                    if(child.stat.filetype == PHYSFS_FILETYPE_DIRECTORY)
                    {
                        auto subfiles = PHYSFS_enumerateFiles(
                            child.fullpath.c_str()
                        );
                        if(subfiles)
                        {
                            try
                            {
                                child.BuildChildren(subfiles);
                            }
                            catch(...)
                            {
                                PHYSFS_freeList(subfiles);
                                throw;
                            }
                            PHYSFS_freeList(subfiles);
                        }
                    }

                    children.emplace_back(
                        std::move(child)
                    );
                }
            }

            const std::string& GetInfo()
            {
                if(info)
                    return *info;

                const char* type_name[] =
                {
                    "regular",
                    "directory",
                    "symlink",
                    "other"
                };

                auto time_util = [](std::time_t t)->std::string
                {
                    char buf[32]{};
                    auto tm = std::gmtime(&t);
                    if(!tm)
                        return "unknown";
                    std::strftime(buf, 32, "%Y/%m/%d %H:%M:%S", tm);
                    return buf;
                };
                info = fmt::format(
                    "File type: {} ({})\n"
                    "Full path: {}\n"
                    "Last modified time: {}\n"
                    "Creation time: {}\n"
                    "Last access time: {}\n"
                    "Readonly: {}",
                    type_name[stat.filetype], stat.filetype,
                    fullpath,
                    time_util(stat.modtime),
                    time_util(stat.createtime),
                    time_util(stat.accesstime),
                    stat.readonly ? 'Y' : 'N'
                );

                return *info;
            }
        };

        void Tooltip(CacheNode& node)
        {
            if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::BeginTooltip();
                const auto& info = node.GetInfo();
                ImGui::TextUnformatted(
                    info.c_str(),
                    info.c_str() + info.size()
                );
                ImGui::EndTooltip();
            }
        }

        void RenderTree(CacheNode& node)
        {
            if(node.children.empty())
            {
                ImGui::Text(node.name.c_str());
                Tooltip(node);
            }
            else
            {
                bool opened = ImGui::TreeNode(node.name.c_str());
                Tooltip(node);
                if(opened)
                {
                    for(auto& v : node.children)
                        RenderTree(v);
                    ImGui::TreePop();
                }
            }
        }
    }

    VfsViewer::VfsViewer() = default;

    void VfsViewer::NewFrame()
    {
        if(ImGui::Begin("Virtual Filesystem Viewer"))
        {
            if(!m_root)
            {
                BuildNode();
            }

            detailed::RenderTree(*m_root);
        }
        ImGui::End();
    }

    void VfsViewer::Show() noexcept
    {
        m_open = true;
    }
    void VfsViewer::Hide() noexcept
    {
        m_open = false;
    }

    void VfsViewer::BuildNode()
    {
        assert(!m_root);
        m_root = std::make_shared<detailed::CacheNode>();

        constexpr auto dir = "/";
        auto files = PHYSFS_enumerateFiles(dir);

        auto& root = *m_root; // For convenience
        root.name = "/";
        root.fullpath = "";
        PHYSFS_stat(dir, &root.stat);
        root.BuildChildren(files);

        PHYSFS_freeList(files);
    }
}
