// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_RES_VFS_HPP
#define TESTWORLD_RES_VFS_HPP

#include <string>
#include <vector>
#include <physfs.h>


namespace awe::vfs
{
    class VfsFile
    {
    public:
        enum Mode
        {
            READ,
            WRITE,
            APPEND
        };

        VfsFile();
        VfsFile(const VfsFile&) = delete;
        VfsFile(VfsFile&& move) noexcept
            : m_handle(std::exchange(move.m_handle, nullptr)),
            m_good(std::exchange(move.m_good, false)) {}
        VfsFile(const std::string& filename, Mode mode = READ);

        ~VfsFile() noexcept;

        void Open(const std::string& filename, Mode mode = READ);
        void Close() noexcept;

        std::size_t Read(void* buf, std::size_t count);

        PHYSFS_Stat Stat();

        [[nodiscard]]
        constexpr PHYSFS_File* GetHandle() noexcept { return m_handle; }
        [[nodiscard]]
        operator bool() noexcept { return m_good; }

    private:
        std::string m_filename;
        PHYSFS_File* m_handle = nullptr;
        bool m_good = false;
    };

    std::vector<std::byte> GetData(const std::string& filename);
    std::string GetString(const std::string& filename);
}

#endif
